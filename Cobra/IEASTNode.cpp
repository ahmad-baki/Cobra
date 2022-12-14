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

IEASTNode::IEASTNode(SymbTable* table, std::string_view path, std::string_view text)
	: table{table}, token{Token(TokenType::NONE)}, path{path}, text{text}
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
		if (!table->isVarReg(token.value))
			return SyntaxError("Presumably Missing '('-Bracket", token.line, token.startColumn, token.endColumn, path, text);

		auto [type, error] = table->getRegVar(token.value);
		if (error.m_errorName != "NULL")
			return SyntaxError(error.desc, token.line, token.startColumn, token.endColumn, path, text);
		returnType = type;
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
	return SyntaxError("Contact the Creator: getExpr<T> should not habe been called", token.line, token.startColumn, token.endColumn, path, text);
}

template<>
std::pair<Expression<bool>*, Error> IEASTNode::getExpr() {
	if (returnType != 0 && returnType != 1) {
		return { nullptr,
			SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
		};
	}

	switch (token.type)
	{
	case TokenType::INTLIT:
	case TokenType::DECLIT:
		return { new Literal<bool>(std::stoi(token.value), token.line, token.startColumn, token.endColumn), Error()};
	case TokenType::IDENTIFIER:
		return { new GetVar<bool>(token.value, table, token.line, token.startColumn, token.endColumn), Error() };
	case TokenType::NONE:
		if (leftNode != nullptr) {
			return leftNode->getExpr<bool>();
		}
		if (rightNode != nullptr) {
			return rightNode->getExpr<bool>();
		}	
		return { nullptr,
			SyntaxError("IEASTNode has TokenType: None, but no child-nodes", token.line, token.startColumn, token.endColumn, path, text)
		};

	// operator
	default:
		if (leftNode == nullptr) {
			return { nullptr,
				SyntaxError("Operator Node has no child-elements", token.line, token.startColumn, token.endColumn, path, text)
			};
		}

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

				return { new BinOp<int, int, bool>(leftExpr, rightExpr, token.type, 
					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error()};
			}
			if (rightType == 1) {
				auto [rightExpr, rightError] = rightNode->getExpr<float>();

				if (rightError.m_errorName != "NULL")
					return { nullptr, rightError };
				return { new BinOp<int, float, bool>(leftExpr, rightExpr, token.type,
					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error()};
			}
			else {
				return { nullptr,
					SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
				};
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
				return { new BinOp<float, int, bool>(leftExpr, rightExpr, token.type,
					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error() };
			}
			else if (rightType == 1) {
				auto [rightExpr, rightError] = rightNode->getExpr<float>();
				if (rightError.m_errorName != "NULL")
					return { nullptr, rightError };
				return { new BinOp<float, float, bool>(leftExpr, rightExpr, token.type,
					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error() };
			}
			else {
				return { nullptr,
					SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
				};
			}
		}
		else {
			return { nullptr,
				SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
			};
		}
	}
}

template<>
std::pair <Expression<int>*, Error> IEASTNode::getExpr() {
	if (returnType != 1 && returnType != 0){
		return { nullptr,
			SyntaxError("implicit Cast to int not possible", token.line, token.startColumn, token.endColumn, path, text)
		};
	}

	switch (token.type)
	{
	case TokenType::INTLIT:
	case TokenType::DECLIT:
		return { new Literal<int>(std::stoi(token.value), token.line, token.startColumn, token.endColumn), Error() };
	case TokenType::IDENTIFIER:
		return { new GetVar<int>(token.value, table, token.line, token.startColumn, token.endColumn), Error() };

	default:
		if (leftNode == nullptr) {
			return { nullptr,
				SyntaxError("Operator Node has no child-elements", token.line, token.startColumn, token.endColumn, path, text)
			};
		}

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

				return { new BinOp<int, int, int>(leftExpr, rightExpr, token.type,
					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error()};
			}
			else {
				return { nullptr,
					SyntaxError("invalid 2. type", token.line, token.startColumn, token.endColumn, path, text)
				};
			}
		}
		else {
			return { nullptr,
				SyntaxError("invalid 1. type", token.line, token.startColumn, token.endColumn, path, text)
			};
		}
	}
}

template<>
std::pair<Expression<float>*, Error> IEASTNode::getExpr() {
	if (returnType != 1 && returnType != 0) {
		return { nullptr,
			SyntaxError("implicit Cast to float not possible", token.line, token.startColumn, token.endColumn, path, text)
		};
	}

	switch (token.type)
	{
	case TokenType::INTLIT:
	case TokenType::DECLIT:
		return { new Literal<float>(std::stof(token.value), token.line, token.startColumn, token.endColumn), Error() };
	case TokenType::IDENTIFIER:
		return { new GetVar<float>(token.value, table, token.line, token.startColumn, token.endColumn), Error() };

	// operator
	default:
		if (leftNode == nullptr){
			return { nullptr,
				SyntaxError("Operator Node has no child-elements", token.line, token.startColumn, token.endColumn, path, text)
			};
		}

		auto [leftExpr, leftError] = leftNode->getExpr<float>();

		if (leftError.m_errorName != "NULL")
			return { nullptr, leftError };

		if (rightNode == nullptr)
			return { leftExpr, Error() };

		auto [rightExpr, rightError] = rightNode->getExpr<float>();

		if (rightError.m_errorName != "NULL")
			return { nullptr, rightError };

		return { new BinOp<float, float, float>(leftExpr, rightExpr, token.type,
					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error() };
	}
}