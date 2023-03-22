#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "IEASTNode.h"
#include "TokenType.cpp"
#include "Literal.h"
#include "GetVal.h"
#include "BinOp.h"
#include "SyntaxError.h"
#include "Interpreter.h"
#include "PrimValue.h"
#include "ListExpr.h"


IEASTNode::IEASTNode(std::string_view path, std::string_view text)
	: token{Token(TokenType::NONE)}, path{path}, text{text}
{
}


IEASTNode::~IEASTNode() {
	if (leftNode != nullptr)
		delete leftNode;
	if (rightNode != nullptr)
		delete rightNode;
}

Expression* IEASTNode::getExpr(Error& outError) {
	Expression* expr{nullptr};
	Interpreter* interp = Interpreter::getSingelton();
	switch (token.dataType)
	{
	case TokenType::INTLIT:
		expr = new PrimValue(interp->getTypeId("int", outError), 
			(void*)new int{std::stoi(token.value)}, outError);
		break;
	case TokenType::DECLIT:
		expr = new PrimValue(interp->getTypeId("float", outError), 
			(void*)new float{ std::stof(token.value) }, outError);
		break;
	case TokenType::CHARLIT:
		expr = new PrimValue(interp->getTypeId("char", outError),
			(void*)new char{ token.value.at(0) }, outError);
		break;
	case TokenType::IDENTIFIER:
		{
			Expression* indexExpr{ nullptr };

			if (leftNode != nullptr) {
				indexExpr = leftNode->getExpr(outError);
			}
			expr = new GetVar(token.value, indexExpr, 
				token.line, token.startColumn, token.endColumn);
		}
		break;
	case TokenType::LCURLBRACKET:
	case TokenType::COMMA:
	{
		ListExpr* listExpr = new ListExpr();
		if (leftNode != nullptr) {
			listExpr->add(leftNode->getExpr(outError));
			listExpr->add((ListExpr*)rightNode->getExpr(outError));
		}
		expr = listExpr;
		break;
	}
	case TokenType::RCURLBRACKET:
		expr = new ListExpr();
		break;
	default:
		if (leftNode == nullptr) {
			SyntaxError targetError = SyntaxError("Operator Node has no child-elements", token.line,
				token.startColumn, token.endColumn, path, text);
			outError.copy(targetError);
			return nullptr;
		}
		else {
			Expression* leftExpr = leftNode->getExpr(outError);

			if (leftExpr == nullptr)
				return nullptr;

			if (rightNode == nullptr)
				return leftExpr;

			Expression* rightExpr = rightNode->getExpr(outError);

			if (rightExpr == nullptr)
				return nullptr;

			expr = new BinOp(leftExpr, rightExpr, token.dataType);
		}
		break;
	}

	if (outError.errorName != "NULL") {
		return nullptr;
	}
	expr->line			= token.line;
	expr->startColumn	= token.startColumn;
	expr->endColumn		= token.endColumn;
	return expr;
}

#pragma region OLD-CODE
//Error IEASTNode::setReturnType(SymbTable* table)
//{
//	switch(token.type)
//	{
//	case TokenType::INTLIT:
//		returnType = 0;
//		break;
//	case TokenType::DECLIT:
//		returnType = 1;
//		break;
//	case TokenType::IDENTIFIER:
//	{
//		if (!table->isVarReg(token.value))
//			return SyntaxError("Presumably Missing '('-Bracket", token.line, token.startColumn, token.endColumn, path, text);
//
//		auto [type, error] = table->getRegVar(token.value);
//		if (error.m_errorName != "NULL")
//			return SyntaxError(error.desc, token.line, token.startColumn, token.endColumn, path, text);
//		returnType = type;
//		break;
//	}
//
//// operator
//	default:
//		int leftNodeType{ -1 };
//		int rightNodeType{ -1 };
//
//		if (leftNode != nullptr) {
//			leftNode->setReturnType(table);
//			leftNodeType = leftNode->returnType;
//		}
//		if (rightNode != nullptr) {
//			rightNode->setReturnType(table);
//			rightNodeType = rightNode->returnType;
//		}
//
//		returnType = std::max(leftNodeType, rightNodeType);
//		break;
//	}
//	return Error();
//}
//
//
//template<SuppType T>
//std::pair<Expression<T>*, Error> IEASTNode::getExpr() {
//	//return { nullptr, 
//	//	SyntaxError("Contact the Creator: getExpr<T> should not habe been called", path, text, currentToken.line, currentToken.column - 1, currentToken.column + 1)
//	//};
//	return SyntaxError("Contact the Creator: getExpr<T> should not habe been called", token.line, token.startColumn, token.endColumn, path, text);
//}
//
//template<>
//std::pair<Expression<bool>*, Error> IEASTNode::getExpr() {
//	if (returnType != 0 && returnType != 1) {
//		return { nullptr,
//			SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
//		};
//	}
//
//	switch (token.type)
//	{
//	case TokenType::INTLIT:
//	case TokenType::DECLIT:
//		return { new Literal<bool>(std::stoi(token.value), token.line, token.startColumn, token.endColumn), Error()};
//	case TokenType::IDENTIFIER:
//		return { new GetVar<bool>(token.value, table, token.line, token.startColumn, token.endColumn), Error() };
//	case TokenType::NONE:
//		if (leftNode != nullptr) {
//			return leftNode->getExpr<bool>();
//		}
//		if (rightNode != nullptr) {
//			return rightNode->getExpr<bool>();
//		}	
//		return { nullptr,
//			SyntaxError("IEASTNode has TokenType: None, but no child-nodes", token.line, token.startColumn, token.endColumn, path, text)
//		};
//
//	// operator
//	default:
//		if (leftNode == nullptr) {
//			return { nullptr,
//				SyntaxError("Operator Node has no child-elements", token.line, token.startColumn, token.endColumn, path, text)
//			};
//		}
//
//		int leftType = leftNode->getReturnType();
//		if (leftType == 0) {
//			// nodes run filled from left->right
//			if (rightNode == nullptr)
//				return leftNode->getExpr<bool>();
//
//			auto [leftExpr, leftError] = leftNode->getExpr<int>();
//
//			if (leftError.m_errorName != "NULL")
//				return { nullptr, leftError };
//
//			int rightType = rightNode->getReturnType();
//			if (rightType == 0)
//			{
//				auto [rightExpr, rightError] = rightNode->getExpr<int>();
//
//				if (rightError.m_errorName != "NULL")
//					return { nullptr, rightError };
//
//				return { new BinOp<int, int, bool>(leftExpr, rightExpr, token.type, 
//					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error()};
//			}
//			if (rightType == 1) {
//				auto [rightExpr, rightError] = rightNode->getExpr<float>();
//
//				if (rightError.m_errorName != "NULL")
//					return { nullptr, rightError };
//				return { new BinOp<int, float, bool>(leftExpr, rightExpr, token.type,
//					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error()};
//			}
//			else {
//				return { nullptr,
//					SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
//				};
//			}
//		}
//		if (leftType == 1) {
//			// nodes run filled from left->right
//			if (rightNode == nullptr)
//				return leftNode->getExpr<bool>();
//
//			auto [leftExpr, leftError] = leftNode->getExpr<float>();
//
//			if (leftError.m_errorName != "NULL")
//				return { nullptr, leftError };
//
//			int rightType = rightNode->getReturnType();
//			if (rightType == 0)
//			{
//				auto [rightExpr, rightError] = rightNode->getExpr<int>();
//				if (rightError.m_errorName != "NULL")
//					return { nullptr, rightError };
//				return { new BinOp<float, int, bool>(leftExpr, rightExpr, token.type,
//					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error() };
//			}
//			else if (rightType == 1) {
//				auto [rightExpr, rightError] = rightNode->getExpr<float>();
//				if (rightError.m_errorName != "NULL")
//					return { nullptr, rightError };
//				return { new BinOp<float, float, bool>(leftExpr, rightExpr, token.type,
//					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error() };
//			}
//			else {
//				return { nullptr,
//					SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
//				};
//			}
//		}
//		else {
//			return { nullptr,
//				SyntaxError("implicit Cast to bool not possible", token.line, token.startColumn, token.endColumn, path, text)
//			};
//		}
//	}
//}
//
//template<>
//std::pair <Expression<int>*, Error> IEASTNode::getExpr() {
//	if (returnType != 1 && returnType != 0){
//		return { nullptr,
//			SyntaxError("implicit Cast to int not possible", token.line, token.startColumn, token.endColumn, path, text)
//		};
//	}
//
//	switch (token.type)
//	{
//	case TokenType::INTLIT:
//	case TokenType::DECLIT:
//		return { new Literal<int>(std::stoi(token.value), token.line, token.startColumn, token.endColumn), Error() };
//	case TokenType::IDENTIFIER:
//		return { new GetVar<int>(token.value, table, token.line, token.startColumn, token.endColumn), Error() };
//
//	default:
//		if (leftNode == nullptr) {
//			return { nullptr,
//				SyntaxError("Operator Node has no child-elements", token.line, token.startColumn, token.endColumn, path, text)
//			};
//		}
//
//		int leftType = leftNode->getReturnType();
//		if (leftType == 0){
//			auto [leftExpr, leftError] = leftNode->getExpr<int>();
//
//			if (leftError.m_errorName != "NULL")
//				return { nullptr, leftError };
//
//			if (rightNode == nullptr)
//				return { leftExpr, Error() };
//
//			int rightType = rightNode->getReturnType();
//			if (rightType == 0)
//			{
//				auto [rightExpr, rightError] = rightNode->getExpr<int>();
//
//				if (rightError.m_errorName != "NULL")
//					return { nullptr, leftError };
//
//				return { new BinOp<int, int, int>(leftExpr, rightExpr, token.type,
//					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error()};
//			}
//			else {
//				return { nullptr,
//					SyntaxError("invalid 2. type", token.line, token.startColumn, token.endColumn, path, text)
//				};
//			}
//		}
//		else {
//			return { nullptr,
//				SyntaxError("invalid 1. type", token.line, token.startColumn, token.endColumn, path, text)
//			};
//		}
//	}
//}
//
//template<>
//std::pair<Expression<float>*, Error> IEASTNode::getExpr() {
//	if (returnType != 1 && returnType != 0) {
//		return { nullptr,
//			SyntaxError("implicit Cast to float not possible", token.line, token.startColumn, token.endColumn, path, text)
//		};
//	}
//
//	switch (token.type)
//	{
//	case TokenType::INTLIT:
//	case TokenType::DECLIT:
//		return { new Literal<float>(std::stof(token.value), token.line, token.startColumn, token.endColumn), Error() };
//	case TokenType::IDENTIFIER:
//		return { new GetVar<float>(token.value, table, token.line, token.startColumn, token.endColumn), Error() };
//
//	// operator
//	default:
//		if (leftNode == nullptr){
//			return { nullptr,
//				SyntaxError("Operator Node has no child-elements", token.line, token.startColumn, token.endColumn, path, text)
//			};
//		}
//
//		auto [leftExpr, leftError] = leftNode->getExpr<float>();
//
//		if (leftError.m_errorName != "NULL")
//			return { nullptr, leftError };
//
//		if (rightNode == nullptr)
//			return { leftExpr, Error() };
//
//		auto [rightExpr, rightError] = rightNode->getExpr<float>();
//
//		if (rightError.m_errorName != "NULL")
//			return { nullptr, rightError };
//
//		return { new BinOp<float, float, float>(leftExpr, rightExpr, token.type,
//					leftExpr->line, leftExpr->startColumn, rightExpr->endColumn), Error() };
//	}
//}
#pragma endregion