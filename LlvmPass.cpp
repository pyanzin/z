#include "LlvmPass.h"
#include <llvm/IR/LLVMContext.h>
#include "ZFunc.h"
#include "ZModule.h"
#include "ZArg.h"
#include "ZBasicTypes.h"
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

using namespace llvm;

LlvmPass::LlvmPass() {
	_builder = new IRBuilder<>(getGlobalContext());
	_currentValues = new LlvmTable;
}

void LlvmPass::visit(ZModule* zmodule) {
	_module = new Module("test", getGlobalContext());

	for (ZFunc* func : zmodule->getFunctions())
		addFuncDef(func);

	for (ZFunc* func : zmodule->getFunctions())
		func->accept(this);	
}

void LlvmPass::addFuncDef(ZFunc* zfunc) {
	auto args = std::vector<Type*>();
	for (auto arg : zfunc->_args) {
		auto argType = arg->getType();
		if (dynamic_cast<ZFuncType*>(argType))
			args.push_back(argType->toLlvmType()->getPointerTo());
		else 
			args.push_back(argType->toLlvmType());
	}
	auto funcType = FunctionType::get(zfunc->_returnType->toLlvmType(), args, false);

	auto func = Function::Create(funcType, Function::ExternalLinkage, zfunc->_name->c_str(), _module);


	auto zargs = zfunc->_args;
	unsigned i = 0;
	for (auto& arg : func->args()) {
		auto zarg = zargs[i++];
		arg.setName(*zarg->getName());
	}

	_currentValues->add(*zfunc->_name, func);
}

void LlvmPass::visit(ZFunc* zfunc) {
	if (zfunc->isExtern())
		return;
	
	_currentValues->enter();

	_func = static_cast<Function*>(_currentValues->get(*zfunc->_name));

	auto zargs = zfunc->_args;
	unsigned i = 0;
	for (auto& arg : _func->args()) {
		auto zarg = zargs[i++];
		_currentValues->add(*zarg->getName(), &arg);
	}

	generate(zfunc->_body);

    if (zfunc->_returnType == Void)
    {
        auto bb = _func->getBasicBlockList().end()->getPrevNode();
        _builder->SetInsertPoint(bb);
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
}

BasicBlock* LlvmPass::generate(ZVarDef* zvardef) {
	BasicBlock* bb = makeBB("zvardef");
	_builder->SetInsertPoint(bb);
	Value* init = getValue(zvardef->getInitExpr(), bb);

	Value* alloc = _builder->CreateAlloca(zvardef->getVarType()->toLlvmType(), nullptr, zvardef->getName());

	_builder->CreateStore(init, alloc);
	_currentValues->add(zvardef->getName(), alloc);

	return bb;
}

BasicBlock* LlvmPass::generate(ZReturn* zreturn) {
	BasicBlock* bb = makeBB("zreturn");
	_builder->SetInsertPoint(bb);

	auto retValue = getValue(zreturn->getExpr(), bb);
	_builder->CreateRet(retValue);

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

Value* LlvmPass::getValue(ZExpr* zexpr, BasicBlock* bb) {
	//_builder->SetInsertPoint(bb);

	ZCall* zcall = dynamic_cast<ZCall*>(zexpr);
	if (zcall)
		return getValue(zcall, bb);

	ZId* zid = dynamic_cast<ZId*>(zexpr);
	if (zid)
		return getValue(zid);

	ZBinOp* zbinop = dynamic_cast<ZBinOp*>(zexpr);
	if (zbinop)
		return getValue(zbinop, bb);

	ZIntLit* zintlit = dynamic_cast<ZIntLit*>(zexpr);
	if (zintlit)
		return getValue(zintlit);

	ZAssign* zassign = dynamic_cast<ZAssign*>(zexpr);
	if (zassign)
		return getValue(zassign, bb);
}

Value* LlvmPass::getValue(ZId* zid) {
	Value* val = _currentValues->get(zid->getName());

	if (isa<AllocaInst>(*val))
		return _builder->CreateLoad(val);	

	return val;
}

Value* LlvmPass::getValue(ZBinOp* zbinop, BasicBlock* bb) {
	_builder->SetInsertPoint(bb);
	auto left = getValue(zbinop->getLeft(), bb);
	auto right = getValue(zbinop->getRight(), bb);

	switch (zbinop->getOp()) {
	case Sum:
		return _builder->CreateAdd(left, right);
	case Sub:
		return _builder->CreateSub(left, right);
	case Mul:
		return _builder->CreateMul(left, right);
	case Div:
		return _builder->CreateSDiv(left, right);
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
	default:
		return nullptr;
	}
}

Value* LlvmPass::getValue(ZIntLit* zintlit) {
	return ConstantInt::get(getGlobalContext(), APInt::APInt(32, zintlit->getValue()));
}

Value* LlvmPass::getValue(ZCall* zcall, BasicBlock* bb) {
	auto callee = getValue(zcall->callee, bb);

	auto args = new std::vector<Value*>();
	for (auto arg : zcall->getArgs())
		args->push_back(getValue(arg, bb));
	return _builder->CreateCall(callee, *args, "");
}

Value* LlvmPass::getValue(ZAssign* zassign, BasicBlock* bb) {
	Value* rightValue = getValue(zassign->getRight(), bb);
	auto alloc = _currentValues->getAlloca(dynamic_cast<ZId*>(zassign->getLeft())->getName());
	
	_builder->CreateStore(rightValue, alloc);

	return rightValue;
}

BasicBlock* LlvmPass::makeBB(std::string name) {
    return _lastBB = BasicBlock::Create(getGlobalContext(), name, _func);
}

BasicBlock* LlvmPass::makeNopBB(std::string name) {
    auto bb = BasicBlock::Create(getGlobalContext(), name, _func);

    _builder->SetInsertPoint(bb);
    Value* nop = _builder->CreateAdd(
        ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0),
        ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0), "nop");

    Value* alloc = _builder->CreateAlloca(nop->getType(), nullptr, "nopa");

    Value* nopStore = _builder->CreateStore(nop, alloc);

    return _lastBB = bb;

}