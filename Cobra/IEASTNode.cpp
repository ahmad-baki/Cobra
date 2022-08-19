#include <exception>
#include <algorithm>
#include "IEASTNode.h"
#include "TokenType.cpp"
#include "Literal.h"
#include "GetVar.h"
#include "BinOp.h"

int IEASTNode::getReturnType() { 
	return returnType; 
}

IEASTNode::IEASTNode(SymbTable* table)
	: table{table}, token{Token(TokenType::NONE)}
{
}

void IEASTNode::setReturnType(SymbTable* table)
{
	switch (token._type)
	{
	// value
	case TokenType::INTLIT:
		returnType = 0;
		break;
	case TokenType::DECLIT:
		returnType = 1;
		break;
	case TokenType::IDENTIFIER:
		returnType = table->varReg[token._value];
		break;

	// operator
	default:
		int leftNodeType{ -1 };
		int rightNodeType{ -1 };

		if (leftNode != nullptr)
			leftNode->setReturnType(table);
			leftNodeType = leftNode->returnType;
		if (rightNode != nullptr)
			rightNode->setReturnType(table);
			rightNodeType = rightNode->returnType;

		returnType = std::max(leftNodeType, rightNodeType);
		break;
	}
}


template<SuppType T>
Expression<T>* IEASTNode::getExpr() {
	return nullptr;
}

template<>
Expression<bool>* IEASTNode::getExpr() {
	if (returnType != 0)
		return nullptr;

	switch (token._type)
	{
	case TokenType::INTLIT:
		return new Literal<bool>(std::stoi(token._value));
	case TokenType::DECLIT:
		return new Literal<bool>(std::stoi(token._value));
	case TokenType::IDENTIFIER:
		return new GetVar<bool>(token._value, table);

	default:
		if (leftNode == nullptr)
			return nullptr;
		int leftType = leftNode->getReturnType();
		if (leftType == 0) {
			// nodes get filled from left->right
			if (rightNode == nullptr)
				return leftNode->getExpr<bool>();

			Expression<int>* leftExpr = leftNode->getExpr<int>();

			int rightType = rightNode->getReturnType();
			if (rightType == 0)
			{
				Expression<int>* rightExpr = rightNode->getExpr<int>();
				return new BinOp<int, int, bool>(leftExpr, rightExpr, token._type);
			}
			if (rightType == 1) {
				Expression<float>* rightExpr = rightNode->getExpr<float>();
				return new BinOp<int, float, bool>(leftExpr, rightExpr, token._type);
			}
			else {
				return nullptr;
			}
		}
		if (leftType == 1) {
			// nodes get filled from left->right
			if (rightNode == nullptr)
				return leftNode->getExpr<bool>();

			Expression<float>* leftExpr = leftNode->getExpr<float>();

			int rightType = rightNode->getReturnType();
			if (rightType == 0)
			{
				Expression<int>* rightExpr = rightNode->getExpr<int>();
				return new BinOp<float, int, bool>(leftExpr, rightExpr, token._type);
			}
			else if (rightType == 1) {
				Expression<float>* rightExpr = rightNode->getExpr<float>();
				return new BinOp<float, float, bool>(leftExpr, rightExpr, token._type);
			}
			else {
				return nullptr;
			}
		}
		else {
			return nullptr;
		}
	}
}

template<>
Expression<int>* IEASTNode::getExpr() {
	if (returnType != 1 && returnType != 0)
		return nullptr;

	switch (token._type)
	{
	case TokenType::INTLIT:
		return new Literal<int>(std::stoi(token._value));
	case TokenType::DECLIT:
		return new Literal<int>(std::stoi(token._value));
	case TokenType::IDENTIFIER:
		return new GetVar<int>(token._value, table);

	default:
		if (leftNode == nullptr)
			return nullptr;
		int leftType = leftNode->getReturnType();
		if (leftType == 0){
			Expression<int>* leftExpr = leftNode->getExpr<int>();
			if (rightNode == nullptr)
				return leftExpr;

			int rightType = rightNode->getReturnType();
			if (rightType == 0)
			{
				Expression<int>* rightExpr = rightNode->getExpr<int>();
				return new BinOp<int, int, int>(leftExpr, rightExpr, token._type);
			}
			else {
				return nullptr;
			}
		}
		else {
			return nullptr;
		}
	}
}

template<>
Expression<float>* IEASTNode::getExpr() {
	if (returnType != 1 && returnType != 0)
		return nullptr;

	switch (token._type)
	{
	case TokenType::INTLIT:
		return new Literal<float>(std::stof(token._value));
	case TokenType::DECLIT:
		return new Literal<float>(std::stof(token._value));
	case TokenType::IDENTIFIER:
		return new GetVar<float>(token._value, table);

	// operator
	default:
		if (leftNode == nullptr || rightNode == nullptr)
			return nullptr;

		//int leftType = leftNode->getReturnType();
		//int rightType = rightNode->getReturnType();
		Expression<float>* leftExpr = leftNode->getExpr<float>();
		Expression<float>* rightExpr = rightNode->getExpr<float>();
		return new BinOp<float, float, float>(leftExpr, rightExpr, token._type);


		/*if (leftType == 0) {
			Expression<int>* leftExpr = leftNode->getExpr<int>();
			if (rightType == 1)
			{
				Expression<float>* rightExpr = rightNode->getExpr<float>();
				return new BinOp<int, float, float>(leftExpr, rightExpr, token._type);
			}
			else {
				return nullptr;
			}
		}
		if (leftType == 1) {
			Expression<float>* leftExpr = leftNode->getExpr<float>();
			if (rightType == 0)
			{
				Expression<int>* rightExpr = rightNode->getExpr<int>();
				return new BinOp<float, int, float>(leftExpr, rightExpr, token._type);
			}
			if (rightType == 1)
			{
				Expression<float>* rightExpr = rightNode->getExpr<float>();
				return new BinOp<float, float, float>(leftExpr, rightExpr, token._type);
			}
			else {
				return nullptr;
			}
		}
		else {
			return nullptr;
		}*/
	}
}