#include "LlvmPass.h"
#include <llvm/IR/LLVMContext.h>
#include "ZFunc.h"
#include "ZModule.h"
#include "ZArg.h"
#include "ZBasicType.h"
#include "ZBlock.h"
#include "ZId.h"
#include "ZIf.h"
#include "ZBinOp.h"
#include "ZVarDef.h"
#include "ZIntLit.h"
#include "ZCall.h"
#include "ZReturn.h"
#include "llvm/IR/BasicBlock.h"
#include "ZWhile.h"
#include "ZAssign.h"
#include "ZCharLit.h"
#include "ZStringLit.h"
#include "ZBooleanLit.h"
#include "ZNop.h"
#include "ZCast.h"
#include "ZSubscript.h"
#include "ZLambda.h"
#include "ZFor.h"
#include "ZArrayType.h"
#include "ZSelector.h"
#include "ZSizeOf.h"
#include "ZUnaryOp.h"
#include "ZDoubleLit.h"

using namespace llvm;

LlvmPass::LlvmPass() {
    _builder = new IRBuilder<>(getLlvmContext());
    _currentValues = new LlvmTable;
    _lambdaCounter = 0;
}

void LlvmPass::visit(ZModule* zmodule) {
    _zmodule = zmodule;
    _module = new Module("test", getLlvmContext());
    _diBuilder = new DIBuilder(*_module);

    for (ZFunc* func : zmodule->getFunctions())
        if (!func->isGeneric())
            addFuncDef(func);

    for (ZFunc* func : zmodule->getFunctions())
        if (!func->isGeneric())
            generate(func);

    for (ZClassDef* c : zmodule->classes)
        for (auto method : c->methods)
            addFuncDef(method);

    for (ZClassDef* c : zmodule->classes)
        for (auto method : c->methods)
            generate(method);
}

Value* LlvmPass::addFuncDef(ZFunc* zfunc, string* name) {
    auto args = std::vector<Type*>();
    for (auto arg : zfunc->getArgs()) {
        auto argType = resolve(arg->getType());
        args.push_back(argType->toLlvmType());
    }
    auto funcType = FunctionType::get(resolve(zfunc->getReturnType())->toLlvmType(), args, false);

    auto func = Function::Create(funcType, Function::ExternalLinkage, name ? *name : *zfunc->getName(), _module);

    auto zargs = zfunc->getArgs();
    unsigned i = 0;
    for (auto& arg : func->args()) {
        auto zarg = zargs[i++];
        arg.setName(*zarg->getName());
    }

    _currentValues->add(name ? *name : *zfunc->getName(), func);

    return func;
}

void LlvmPass::generate(ZFunc* zfunc, string* name) {
    if (zfunc->isExtern())
        return;

    _currentValues->enter();

    _func = static_cast<Function*>(_currentValues->get(name ? *name : *zfunc->getName()));

    auto zargs = zfunc->getArgs();
    unsigned i = 0;
    for (auto& arg : _func->args()) {
        auto zarg = zargs[i++];
        _currentValues->add(*zarg->getName(), &arg);
    }

    generate(zfunc->getBody());

    if (resolve(zfunc->getReturnType()) == Void) {
        BasicBlock* lastBb;


        auto iter = _func->getBasicBlockList().begin();
        auto end = _func->getBasicBlockList().end();

        do {
            lastBb = &(*iter);
        }
        while ((++iter) != end);

        _builder->SetInsertPoint(lastBb);
        _builder->CreateRetVoid();
    }

    _currentValues->exit();
}


BasicBlock* LlvmPass::generate(ZBlock* zblock) {
    BasicBlock* startBB = makeBB("zblock");
    BasicBlock* bb = startBB;

    for (ZAst* stmt : zblock->getStatements()) {
        auto newBB = generate(stmt);
        _builder->SetInsertPoint(bb);
        _builder->CreateBr(newBB);
        bb = _lastBB;
    }

    return startBB;
}

BasicBlock* LlvmPass::generate(ZAst* zast) {
    ZExpr* zexpr = dynamic_cast<ZExpr*>(zast);
    if (zexpr) {
        BasicBlock* bb = makeBB("zexpr");
        _builder->SetInsertPoint(bb);
        getValue(zexpr, bb);
        return bb;
    }

    ZBlock* zblock = dynamic_cast<ZBlock*>(zast);
    if (zblock)
        return generate(zblock);


    ZVarDef* zvardef = dynamic_cast<ZVarDef*>(zast);
    if (zvardef)
        return generate(zvardef);

    ZReturn* zreturn = dynamic_cast<ZReturn*>(zast);
    if (zreturn)
        return generate(zreturn);

    ZIf* zif = dynamic_cast<ZIf*>(zast);
    if (zif)
        return generate(zif);

    ZWhile* zwhile = dynamic_cast<ZWhile*>(zast);
    if (zwhile)
        return generate(zwhile);

    ZFor* zfor = dynamic_cast<ZFor*>(zast);
    if (zfor)
        return generate(zfor);

    ZNop* znop = dynamic_cast<ZNop*>(zast);
    if (znop)
        return generate(znop);
}

BasicBlock* LlvmPass::generate(ZVarDef* zvardef) {
    BasicBlock* bb = makeBB("zvardef");

    _builder->SetInsertPoint(bb);
    Value* init = getValue(zvardef->getInitExpr(), bb);

    auto alloc = _builder->CreateAlloca(init->getType(), nullptr, zvardef->getName());

    _builder->CreateStore(init, alloc);
    _currentValues->add(zvardef->getName(), alloc);

    return bb;
}

BasicBlock* LlvmPass::generate(ZReturn* zreturn) {
    BasicBlock* bb = makeBB("zreturn");
    _builder->SetInsertPoint(bb);

    auto expr = zreturn->getExpr();

    if (expr)
        _builder->CreateRet(getValue(expr, bb));
    else
        _builder->CreateRetVoid();

    return bb;
}

BasicBlock* LlvmPass::generate(ZIf* zif) {
    BasicBlock* condBB = makeBB("if_cond");
    Value* condValue = getValue(zif->getCondition(), condBB);
    BasicBlock* bodyBB = generate(zif->getBody());
    auto lastBodyBB = _lastBB;

    llvm::BasicBlock* elseBB = nullptr;
    llvm::BasicBlock* lastElseBB = nullptr;
    if (zif->getElseBody()) {
        elseBB = generate(zif->getElseBody());

        lastElseBB = _lastBB;
    }

    BasicBlock* afterBB = makeBB("after_if");

    _builder->SetInsertPoint(lastBodyBB);
    _builder->CreateBr(afterBB);

    if (lastElseBB) {
        _builder->SetInsertPoint(lastElseBB);
        _builder->CreateBr(afterBB);
    }

    _builder->SetInsertPoint(condBB);
    _builder->CreateCondBr(condValue, bodyBB, elseBB ? elseBB : afterBB);

    return condBB;
}

BasicBlock* LlvmPass::generate(ZWhile* zwhile) {
    BasicBlock* condBB = makeBB("while_cond");

    Value* condValue = getValue(zwhile->getCondition(), condBB);

    BasicBlock* bodyBB = generate(zwhile->getBody());

    _builder->SetInsertPoint(_lastBB);
    _builder->CreateBr(condBB);

    BasicBlock* afterBB = makeNopBB("after_while");

    _builder->SetInsertPoint(condBB);
    _builder->CreateCondBr(condValue, bodyBB, afterBB);

    return condBB;
}

BasicBlock* LlvmPass::generate(ZFor* zfor) {
    BasicBlock* preBB = generate(zfor->getPre());

    BasicBlock* condBB = makeBB("for_cond");
    Value* condValue = getValue(zfor->getCond(), condBB);

    BasicBlock* postBB = makeBB("for_post");
    if (zfor->getPost())
        getValue(zfor->getPost(), postBB);

    BasicBlock* bodyBB = generate(zfor->getBody());
    BasicBlock* bodyLastBB = _lastBB;

    BasicBlock* finalBB = makeNopBB("for_final");

    _builder->SetInsertPoint(preBB);
    _builder->CreateBr(condBB);

    _builder->SetInsertPoint(condBB);
    _builder->CreateCondBr(condValue, bodyBB, finalBB);

    _builder->SetInsertPoint(bodyLastBB);
    _builder->CreateBr(postBB);

    _builder->SetInsertPoint(postBB);
    _builder->CreateBr(condBB);

    _lastBB = finalBB;

    return preBB;
}

BasicBlock* LlvmPass::generate(ZNop* znop) {
    return makeBB("nop");
}

Value* LlvmPass::getValue(ZExpr* zexpr, BasicBlock* bb) {
    _builder->SetInsertPoint(bb);

    ZCall* zcall = dynamic_cast<ZCall*>(zexpr);
    if (zcall)
        return getValue(zcall, bb);

    ZId* zid = dynamic_cast<ZId*>(zexpr);
    if (zid)
        return getValue(zid);

    ZBinOp* zbinop = dynamic_cast<ZBinOp*>(zexpr);
    if (zbinop)
        return getValue(zbinop, bb);

    ZUnaryOp* zunaryop = dynamic_cast<ZUnaryOp*>(zexpr);
    if (zunaryop)
        return getValue(zunaryop, bb);

    ZIntLit* zintlit = dynamic_cast<ZIntLit*>(zexpr);
    if (zintlit)
        return getValue(zintlit);

    ZDoubleLit* zdoublelit = dynamic_cast<ZDoubleLit*>(zexpr);
    if (zdoublelit)
        return getValue(zdoublelit);

    ZCharLit* zcharlit = dynamic_cast<ZCharLit*>(zexpr);
    if (zcharlit)
        return getValue(zcharlit);

    ZBooleanLit* zbooleanlit = dynamic_cast<ZBooleanLit*>(zexpr);
    if (zbooleanlit)
        return getValue(zbooleanlit);

    ZStringLit* zstringlit = dynamic_cast<ZStringLit*>(zexpr);
    if (zstringlit)
        return getValue(zstringlit);

    ZAssign* zassign = dynamic_cast<ZAssign*>(zexpr);
    if (zassign)
        return getValue(zassign, bb);

    ZCast* zcast = dynamic_cast<ZCast*>(zexpr);
    if (zcast)
        return getValue(zcast, bb);

    ZLambda* zlambda = dynamic_cast<ZLambda*>(zexpr);
    if (zlambda)
        return getValue(zlambda);

    ZSubscript* zsubscript = dynamic_cast<ZSubscript*>(zexpr);
    if (zsubscript)
        return getValue(zsubscript, bb);

    ZSelector* zselector = dynamic_cast<ZSelector*>(zexpr);
    if (zselector)
        return getValue(zselector, bb);

    ZSizeOf* zsizeof = dynamic_cast<ZSizeOf*>(zexpr);
    if (zsizeof)
        return getValue(zsizeof, bb);
}

llvm::Value* LlvmPass::getValue(ZCast* zcast, BasicBlock* bb) {
    Value* exprValue = getValue(zcast->getExpr(), bb);
    return _builder->CreateCast(Instruction::BitCast, exprValue, resolve(zcast->getTargetType())->toLlvmType());
}

llvm::Value* LlvmPass::getValue(ZSizeOf* zsizeof, llvm::BasicBlock* bb) {
    _builder->SetInsertPoint(bb);

    Type* resolvedType;
    if (zsizeof->isComplex)
        resolvedType = dynamic_cast<ZStructType*>(zsizeof->getWrappedType())->getStructType();
    else
        resolvedType = resolve(zsizeof->getWrappedType())->toLlvmType();
    DataLayout dataLayout = DataLayout(_module);
    int typeSize = dataLayout.getTypeAllocSize(resolvedType);
    return ConstantInt::get(getLlvmContext(), APInt(32, typeSize));
}

Value* LlvmPass::getValue(ZId* zid) {
    Value* val = _currentValues->get(zid->getName());

    if (isa<AllocaInst>(*val))
        return _builder->CreateLoad(val);

    return val;
}

Value* LlvmPass::getValue(ZSubscript* zsubscript, BasicBlock* bb) {
    _builder->SetInsertPoint(bb);
    Value* targetValue = getValue(zsubscript->getTarget(), bb);
    Value* indexValue = getValue(zsubscript->getIndex(), bb);

    auto gep = _builder->CreateGEP(targetValue, indexValue);
    return _builder->CreateLoad(gep);
}

Value* LlvmPass::getValue(ZSelector* zselector, BasicBlock* bb) {
    _builder->SetInsertPoint(bb);
    Value* targetValue = getValue(zselector->getTarget(), bb);

    int index = zselector->getMemberIndex();

    auto gepArgs = vector<Value*>() = {
        ConstantInt::get(getLlvmContext(), APInt(32, 0)),
        ConstantInt::get(getLlvmContext(), APInt(32, index))
    };

    auto gep = _builder->CreateGEP(targetValue, gepArgs);
    return _builder->CreateLoad(gep);
}

Value* LlvmPass::getValue(ZBinOp* zbinop, BasicBlock* bb) {
    _builder->SetInsertPoint(bb);
    auto left = getValue(zbinop->getLeft(), bb);
    auto right = getValue(zbinop->getRight(), bb);

    if (zbinop->getLeft()->getType()->isEqual(*Double)) {
        switch (zbinop->getOp()) {
        case Sum:
            return _builder->CreateFAdd(left, right);
        case Sub:
            return _builder->CreateFSub(left, right);
        case Mul:
            return _builder->CreateFMul(left, right);
        case Div:
            return _builder->CreateFDiv(left, right);
        case Mod:
            return _builder->CreateFRem(left, right);
        case Equal:
            return _builder->CreateFCmpOEQ(left, right);
        case NotEqual:
            return _builder->CreateFCmpONE(left, right);
        case LessOrEqual:
            return _builder->CreateFCmpOLE(left, right);
        case Less:
            return _builder->CreateFCmpOLT(left, right);
        case MoreOrEqual:
            return _builder->CreateFCmpOGE(left, right);
        case More:
            return _builder->CreateFCmpOGT(left, right);
        default:
            return nullptr;
        }
    }
    switch (zbinop->getOp()) {
    case Sum:
        return _builder->CreateAdd(left, right);
    case Sub:
        return _builder->CreateSub(left, right);
    case Mul:
        return _builder->CreateMul(left, right);
    case Div:
        return _builder->CreateSDiv(left, right);
    case Mod:
        return _builder->CreateSRem(left, right);
    case Equal:
        return _builder->CreateICmpEQ(left, right);
    case NotEqual:
        return _builder->CreateICmpNE(left, right);
    case LessOrEqual:
        return _builder->CreateICmpSLE(left, right);
    case Less:
        return _builder->CreateICmpSLT(left, right);
    case MoreOrEqual:
        return _builder->CreateICmpSGE(left, right);
    case More:
        return _builder->CreateICmpSGT(left, right);
    case BooleanOr:
    case BitwiseOr:
        return _builder->CreateOr(left, right);
    case BooleanAnd:
    case BitwiseAnd:
        return _builder->CreateAnd(left, right);
    case BooleanXor:
        return _builder->CreateXor(left, right);

    default:
        return nullptr;
    }
}

llvm::Value* LlvmPass::getValue(ZUnaryOp* zunaryop, llvm::BasicBlock* bb) {
    _builder->SetInsertPoint(bb);
    auto targetValue = getValue(zunaryop->getTarget(), bb);
    UnaryOps op = zunaryop->getOp();

    switch (op) {
    case Negation:
    case BitwiseInvert:
        return _builder->CreateNot(targetValue);
    case UnaryPlus:
        return targetValue;
    case UnaryMinus:
        return _builder->CreateNeg(targetValue);
    }

    ZType* targetType = zunaryop->getTarget()->getType();
    bool isDouble = targetType->isEqual(*Double);
    int valueToAdd = op == PreIncrement || op == PostIncrement
                         ? 1
                         : -1;
    Value* sum;
    if (isDouble)
        sum = _builder->CreateFAdd(targetValue, ConstantFP::get(targetValue->getType(), valueToAdd));
    else
        sum = _builder->CreateAdd(targetValue, ConstantInt::get(targetValue->getType(), valueToAdd));

    auto left = getLeftHand(zunaryop->getTarget(), bb);
    _builder->CreateStore(sum, left);

    if (op == PreIncrement || op == PreDecrement)
        return getValue(zunaryop->getTarget(), bb);
    else
        return targetValue;

}

Value* LlvmPass::getValue(ZStringLit* zstringlit) {
    return _builder->CreateGlobalStringPtr(zstringlit->getValue());
}

Value* LlvmPass::getValue(ZIntLit* zintlit) {
    return ConstantInt::get(getLlvmContext(), APInt::APInt(32, zintlit->getValue()));
}

Value* LlvmPass::getValue(ZDoubleLit* zdoublelit) {
    return ConstantFP::get(getLlvmContext(), APFloat::APFloat(zdoublelit->getValue()));
}

Value* LlvmPass::getValue(ZCharLit* zcharlit) {
    return ConstantInt::get(getLlvmContext(), APInt::APInt(8, zcharlit->getValue()));
}

Value* LlvmPass::getValue(ZBooleanLit* zbooleanlit) {
    return ConstantInt::get(getLlvmContext(), APInt::APInt(1, zbooleanlit->getValue() ? 1 : 0));
}

Value* LlvmPass::generateConcrete(ZFunc* func, SymbolRef* symbolRef) {
    _resolutionChain.push(symbolRef);
    auto oldLastBB = _lastBB;
    auto oldfunc = _func;

    auto name = new string(*func->getName());

    for (auto gen : func->getTypeParams()) {
        *name += resolve(gen)->getName();
    }

    auto alreadyExisting = _module->getFunction(*name);
    if (alreadyExisting)
        return alreadyExisting;

    Value* result = addFuncDef(func, name);
    if (!func->isExtern())
        generate(func, name);

    _resolutionChain.pop();
    _lastBB = oldLastBB;
    _func = oldfunc;
    _builder->SetInsertPoint(_lastBB);

    return result;
}

Value* LlvmPass::getValue(ZCall* zcall, BasicBlock* bb) {
    ZFuncType* calleeType = dynamic_cast<ZFuncType*>(zcall->callee->getType());

    ZId* zid = dynamic_cast<ZId*>(zcall->callee);

    ZFunc* func = _zmodule->findFunc(zid->getName());
    llvm::Value* callee;
    if (calleeType->hasGenericDefs())
        callee = generateConcrete(func, zcall->getRef());
    else
        callee = getValue(zcall->callee, bb);

    _builder->SetInsertPoint(bb);

    auto args = new std::vector<Value*>();
    for (auto arg : zcall->getArgs()) {
        auto value = getValue(arg, bb);
        args->push_back(value);
    }
    return _builder->CreateCall(callee, *args, "");
}

Value* LlvmPass::getValue(ZAssign* zassign, BasicBlock* bb) {
    Value* rightValue = getValue(zassign->getRight(), bb);
    _builder->CreateStore(rightValue, getLeftHand(zassign->getLeft(), bb));
    return rightValue;
}

Value* LlvmPass::getLeftHand(ZExpr* zexpr, BasicBlock* bb) {
    if (dynamic_cast<ZId*>(zexpr)) {
        return _currentValues->getAlloca(dynamic_cast<ZId*>(zexpr)->getName());
    }

    ZSubscript* zsubscript = dynamic_cast<ZSubscript*>(zexpr);
    if (zsubscript) {
        Value* targetValue = getValue(zsubscript->getTarget(), bb);
        Value* indexValue = getValue(zsubscript->getIndex(), bb);
        auto gep = _builder->CreateGEP(targetValue, indexValue);

        return gep;
    }

    ZSelector* zselector = dynamic_cast<ZSelector*>(zexpr);
    if (zselector) {
        Value* targetValue = getValue(zselector->getTarget(), bb);
        int index = zselector->getMemberIndex();
        auto gepArgs = vector<Value*>() = {
            ConstantInt::get(getLlvmContext(), APInt(32, 0)),
            ConstantInt::get(getLlvmContext(), APInt(32, index))
        };
        auto gep = _builder->CreateGEP(targetValue, gepArgs);

        return gep;
    }

    return nullptr;
}

Value* LlvmPass::getValue(ZLambda* zlambda) {
    Function* previousFunc = _func;
    BasicBlock* previousLastBB = _lastBB;

    auto args = std::vector<Type*>();
    for (auto arg : *zlambda->getArgs()) {
        auto argType = resolve(arg->getType());
        args.push_back(argType->toLlvmType());
    }
    auto funcType = FunctionType::get(resolve(zlambda->getReturnType())->toLlvmType(), args, false);

    auto func = Function::Create(
        funcType,
        Function::ExternalLinkage,
        getNextLambdaName(),
        _module);

    _func = func;

    _currentValues->enter();

    vector<ZArg*>* zargs = zlambda->getArgs();
    unsigned i = 0;
    for (auto& arg : func->args()) {
        ZArg* zarg = (*zargs)[i++];
        arg.setName(zarg->getName()->c_str());
        _currentValues->add(*zarg->getName(), &arg);
    }

    ZExpr* exprBody = dynamic_cast<ZExpr*>(zlambda->getBody());

    if (!exprBody) {
        generate(zlambda->getBody());
        _builder->SetInsertPoint(_lastBB);
    }
    else {
        BasicBlock* bb = makeBB("lambda_body");
        Value* retExpr = getValue(exprBody, bb);
        _builder->SetInsertPoint(bb);
        if (!(zlambda->getReturnType()->isEqual(*Void)))
            _builder->CreateRet(retExpr);
    }

    if (zlambda->getReturnType()->isEqual(*Void))
        _builder->CreateRetVoid();

    _currentValues->exit();

    _func = previousFunc;
    _lastBB = previousLastBB;
    _builder->SetInsertPoint(_lastBB);

    return func;
}

ZType* LlvmPass::resolve(ZType* type) {
    return _resolutionChain.resolve(type);
}

BasicBlock* LlvmPass::makeBB(std::string name) {
    return _lastBB = BasicBlock::Create(getLlvmContext(), name, _func);
}

BasicBlock* LlvmPass::makeNopBB(std::string name) {
    auto bb = BasicBlock::Create(getLlvmContext(), name, _func);

    _builder->SetInsertPoint(bb);

    return _lastBB = bb;

}

string& LlvmPass::getNextLambdaName() {
    return *(new string("__lambda" + to_string(_lambdaCounter++)));
}
