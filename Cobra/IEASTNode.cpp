#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "IEASTNode.h"
#include "TokenType.cpp"
#include "Literal.h"
#include "GetVar.h"
#include "BinOp.h"
#include "SyntaxError.h"

int IEASTNode::getReturnType() { 
	return returnType; 
}

IEASTNode::IEASTNode(SymbTable* table)
	: table{table}, token{Token(TokenType::NONE)}
{
}


IEASTNode::~IEASTNode() {
	if (leftNode != nullptr)
		delete leftNode;
	if (rightNode != nullptr)
		delete rightNode;
}


Error IEASTNode::setReturnType(SymbTable* table)
{
	switch(token.type)
	{
	case TokenType::INTLIT:
		returnType = 0;
		break;
	case TokenType::DECLIT:
		returnType = 1;
		break;
	case TokenType::IDENTIFIER:
	{
		auto elem = table->varReg.find(token.value);

		if (elem == table->varReg.end())
			//return SyntaxError("Presumably Missing '('-Bracket", path, text, currentToken.line, currentToken.column - 1, currentToken.column + 1);
			throw std::invalid_argument("variable " + token.value + " is not declared");

		returnType = table->varReg[token.value];
		break;
	}

// operator
	default:
		int leftNodeType{ -1 };
		int rightNodeType{ -1 };

		if (leftNode != nullptr) {
			leftNode->setReturnType(table);
			leftNodeType = leftNode->returnType;
		}
		if (rightNode != nullptr) {
			rightNode->setReturnType(table);
			rightNodeType = rightNode->returnType;
		}

		returnType = std::max(leftNodeType, rightNodeType);
		break;
	}
	return Error();
}


template<SuppType T>
std::pair<Expression<T>*, Error> IEASTNode::getExpr() {
	//return { nullptr, 
	//	SyntaxError("Contact the Creator: getExpr<T> should not habe been called", path, text, currentToken.line, currentToken.column - 1, currentToken.column + 1)
	//};
	throw std::invalid_argument("Contact the Creator: getExpr<T> should not habe been called");
}

template<>
std::pair<Expression<bool>*, Error> IEASTNode::getExpr() {
	if (returnType != 0 && returnType != 1)
		/*return { nullptr,
			SyntaxError("Contact the Creator: getExpr<T> should not habe been called", path, text, currentToken.line, currentToken.column - 1, currentToken.column + 1)
		};*/
		throw std::invalid_argument("implicit Cast to bool not possible");

	switch (token.type)
	{
	case TokenType::INTLIT:
	case TokenType::DECLIT:
		return { new Literal<bool>(std::stoi(token.value)), Error()};
	case TokenType::IDENTIFIER:
		return { new GetVar<bool>(token.value, table), Error() };
	case TokenType::NONE:
		if (leftNode != nullptr) {
			return leftNode->getExpr<bool>();
		}
		if (rightNode != nullptr) {
			return rightNode->getExpr<bool>();
		}
		throw std::invalid_argument("IEASTNode has TokenType: None, but no child-nodes");

	// operator
	default:
		if (leftNode == nullptr)
			throw std::invalid_argument("Operator Node has no child-elements");

		int leftType = leftNode->getReturnType();
		if (leftType == 0) {
			// nodes get filled from left->right
			if (rightNode == nullptr)
				return leftNode->getExpr<bool>();

			auto [leftExpr, leftError] = leftNode->getExpr<int>();

			if (leftError.m_errorName != "NULL")
				return { nullptr, leftError };

			int rightType = rightNode->getReturnType();
			if (rightType == 0)
			{
				auto [rightExpr, rightError] = rightNode->getExpr<int>();

				if (rightError.m_errorName != "NULL")
					return { nullptr, rightError };

				return { new BinOp<int, int, bool>(leftExpr, rightExpr, token.type), Error()};
			}
			if (rightType == 1) {
				auto [rightExpr, rightError] = rightNode->getExpr<float>();

				if (rightError.m_errorName != "NULL")
					return { nullptr, rightError };
				return { new BinOp<int, float, bool>(leftExpr, rightExpr, token.type), Error()};
			}
			else {
				throw std::invalid_argument("implicit Cast to bool not possible");
			}
		}
		if (leftType == 1) {
			// nodes get filled from left->right
			if (rightNode == nullptr)
				return leftNode->getExpr<bool>();

			auto [leftExpr, leftError] = leftNode->getExpr<float>();

			if (leftError.m_errorName != "NULL")
				return { nullptr, leftError };

			int rightType = rightNode->getReturnType();
			if (rightType == 0)
			{
				auto [rightExpr, rightError] = rightNode->getExpr<int>();
				if (rightError.m_errorName != "NULL")
					return { nullptr, rightError };
				return { new BinOp<float, int, bool>(leftExpr, rightExpr, token.type), Error() };
			}
			else if (rightType == 1) {
				auto [rightExpr, rightError] = rightNode->getExpr<float>();
				if (rightError.m_errorName != "NULL")
					return { nullptr, rightError };
				return { new BinOp<float, float, bool>(leftExpr, rightExpr, token.type), Error() };
			}
			else {
				throw std::invalid_argument("implicit Cast to bool not possible");
			}
		}
		else {
			throw std::invalid_argument("implicit Cast to bool not possible");
		}
	}
}

template<>
std::pair <Expression<int>*, Error> IEASTNode::getExpr() {
	if (returnType != 1 && returnType != 0)
		throw std::invalid_argument("implicit Cast to int not possible");

	switch (token.type)
	{
	case TokenType::INTLIT:
	case TokenType::DECLIT:
		return { new Literal<int>(std::stoi(token.value)), Error() };
	case TokenType::IDENTIFIER:
		return { new GetVar<int>(token.value, table), Error() };

	default:
		if (leftNode == nullptr)
			throw std::invalid_argument("Operator Node has no child-elements");

		int leftType = leftNode->getReturnType();
		if (leftType == 0){
			auto [leftExpr, leftError] = leftNode->getExpr<int>();

			if (leftError.m_errorName != "NULL")
				return { nullptr, leftError };

			if (rightNode == nullptr)
				return { leftExpr, Error() };

			int rightType = rightNode->getReturnType();
			if (rightType == 0)
			{
				auto [rightExpr, rightError] = rightNode->getExpr<int>();

				if (rightError.m_errorName != "NULL")
					return { nullptr, leftError };

				return { new BinOp<int, int, int>(leftExpr, rightExpr, token.type), Error()};
			}
			else {
				throw std::invalid_argument("invalid 2. type");
			}
		}
		else {
			throw std::invalid_argument("invalid 1. type");
		}
	}
}

template<>
std::pair<Expression<float>*, Error> IEASTNode::getExpr() {
	if (returnType != 1 && returnType != 0)
		throw std::invalid_argument("implicit Cast to float not possible");

	switch (token.type)
	{
	case TokenType::INTLIT:
	case TokenType::DECLIT:
		return { new Literal<float>(std::stof(token.value)), Error() };
	case TokenType::IDENTIFIER:
		return { new GetVar<float>(token.value, table), Error() };

	// operator
	default:
		if (leftNode == nullptr)
			throw std::invalid_argument("Operator Node has no child-elements");

		auto [leftExpr, leftError] = leftNode->getExpr<float>();

		if (leftError.m_errorName != "NULL")
			return { nullptr, leftError };

		if (rightNode == nullptr)
			return { leftExpr, Error() };

		auto [rightExpr, rightError] = rightNode->getExpr<float>();

		if (rightError.m_errorName != "NULL")
			return { nullptr, rightError };

		return { new BinOp<float, float, float>(leftExpr, rightExpr, token.type), Error() };
	}
}