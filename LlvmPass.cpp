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
}

void LlvmPass::visit(ZModule* zmodule) {
	_module = new Module("test", getGlobalContext());
	for (ZFunc* func : zmodule->getFunctions()) {
		func->accept(this);
	}
	

	
}

void LlvmPass::visit(ZFunc* zfunc) {
	_currentValues.clear();

	auto args = std::vector<Type*>();
	for (auto arg : zfunc->_args)
		args.push_back(arg->getType()->toLlvmType());
	auto funcType = FunctionType::get(zfunc->_returnType->toLlvmType(), args, false);

	_func = Function::Create(funcType, Function::ExternalLinkage, zfunc->_name->c_str(), _module);

	Function::arg_iterator fargs = _func->arg_begin();
	Value* x = fargs++;
	x->setName("x");
	Value* y = fargs++;
	y->setName("y");

	/*BasicBlock* entry = BasicBlock::Create(getGlobalContext(), "", _func);
	BasicBlock* ret = BasicBlock::Create(getGlobalContext(), "", _func);
	BasicBlock* cond_false = BasicBlock::Create(getGlobalContext(), "", _func);
	BasicBlock* cond_true = BasicBlock::Create(getGlobalContext(), "", _func);
	BasicBlock* cond_false_2 = BasicBlock::Create(getGlobalContext(), "", _func);

	IRBuilder<> builder(entry);
	Value* xEqualsY = builder.CreateICmpEQ(x, y, "tmp");
	builder.CreateCondBr(xEqualsY, ret, cond_false);

	builder.SetInsertPoint(ret);
	builder.CreateRet(x);

	builder.SetInsertPoint(cond_false);
	Value* xLessThanY = builder.CreateICmpULT(x, y, "tmp");
	builder.CreateCondBr(xLessThanY, cond_true, cond_false_2);

	builder.SetInsertPoint(cond_true);
	Value* yMinusX = builder.CreateSub(y, x, "tmp");
	std::vector<Value*> args1;
	args1.push_back(x);
	args1.push_back(yMinusX);
	Value* recur_1 = builder.CreateCall(_func, args1, "tmp");
	builder.CreateRet(recur_1);

	builder.SetInsertPoint(cond_false_2);
	Value* xMinusY = builder.CreateSub(x, y, "tmp");
	std::vector<Value*> args2;
	args2.push_back(xMinusY);
	args2.push_back(y);
	llvm::Value* recur_2 = builder.CreateCall(_func, args2, "tmp");
	builder.CreateRet(recur_2);

	return;*/

	_currentValues.clear();

	//auto args = std::vector<Type*>();
	//for (auto arg : zfunc->_args)
	//	args.push_back(arg->getType()->toLlvmType());

	//auto funcType = FunctionType::get(zfunc->_returnType->toLlvmType(), args, false);
	//_func = Function::Create(funcType, Function::ExternalLinkage, zfunc->_name->c_str(), _module);

	auto zargs = zfunc->_args;
	unsigned i = 0;
	for (auto& arg : _func->args()) {
		auto zarg = zargs[i++];
		arg.setName(*zarg->getName());
		_currentValues[*zarg->getName()] = &arg;
	}

	_currentValues[*zfunc->_name] = _func;

	generate(zfunc->_body);
}

BasicBlock* LlvmPass::generate(ZBlock* zblock) {
	BasicBlock* bb = BasicBlock::Create(getGlobalContext(), "", _func);
	_builder->SetInsertPoint(bb);

	for (ZAst* stmt : zblock->getStatements()) {

		BasicBlock* newBB = BasicBlock::Create(getGlobalContext(), "", _func);
		_builder->SetInsertPoint(newBB);

		generate(stmt);
	}

	return bb;
}

BasicBlock* LlvmPass::generate(ZAst* zast) {
	BasicBlock* bb = BasicBlock::Create(getGlobalContext(), "", _func);
	_builder->SetInsertPoint(bb);

	ZExpr* zexpr = dynamic_cast<ZExpr*>(zast);
	if (zexpr) {
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
	BasicBlock* bb = BasicBlock::Create(getGlobalContext(), "", _func);
	_builder->SetInsertPoint(bb);

	Value* init = getValue(zvardef->getInitExpr(), bb);
	_currentValues[zvardef->getName()] = init;

	return bb;
}

BasicBlock* LlvmPass::generate(ZReturn* zreturn) {
	BasicBlock* bb = BasicBlock::Create(getGlobalContext(), "", _func);
	_builder->SetInsertPoint(bb);

	auto retValue = getValue(zreturn->getExpr(), bb);
	_builder->CreateRet(retValue);

	return bb;
}

BasicBlock* LlvmPass::generate(ZIf* zif) {
	BasicBlock* condBB = BasicBlock::Create(getGlobalContext(), "", _func);

	Value* condValue = getValue(zif->getCondition(), condBB);

	BasicBlock* bodyBB = generate(zif->getBody());
	
	BasicBlock* elseBB;

	if (zif->getElseBody())
		elseBB = generate(zif->getElseBody());
	else
		elseBB = BasicBlock::Create(getGlobalContext(), "", _func);

	_builder->CreateCondBr(condValue, bodyBB, elseBB);

	return condBB;
}

BasicBlock* LlvmPass::generate(ZWhile* zwhile) {
	BasicBlock* condBB = BasicBlock::Create(getGlobalContext(), "", _func);
	
	Value* condValue = getValue(zwhile->getCondition(), condBB);

	BasicBlock* bodyBB = generate(zwhile->getBody());

	BasicBlock* afterBB = BasicBlock::Create(getGlobalContext(), "", _func);

	_builder->SetInsertPoint(condBB);
	_builder->CreateCondBr(condValue, bodyBB, afterBB);

	_builder->SetInsertPoint(bodyBB);
	_builder->CreateBr(condBB);

	return condBB;
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

	ZIntLit* zintlit = dynamic_cast<ZIntLit*>(zexpr);
	if (zintlit)
		return getValue(zintlit);

	ZAssign* zassign = dynamic_cast<ZAssign*>(zexpr);
	if (zassign)
		return getValue(zassign, bb);
}

Value* LlvmPass::getValue(ZId* zid) {
	return _currentValues[zid->getName()];
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
	case LessOrEqual:
		return _builder->CreateICmpSLE(left, right);
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
	Value* rightValue = getValue(zassign->_right, bb);
	_currentValues[dynamic_cast<ZId*>(zassign->_left)->getName()] = rightValue;
	return rightValue;
}