#include "IEASTFuncNode.h"
#include "RunFunc.h"
#include <filesystem>

namespace fs = std::filesystem;

IEASTFuncNode::IEASTFuncNode(fs::path path, std::string_view text)
	: IEASTNode(path, text), params{}
{

}

Expression* IEASTFuncNode::getExpr(Error& outError) 
{
	std::vector<Expression*> paramExpr{};
	paramExpr.reserve(params.size());
	for (auto i = params.begin(); i != params.end(); i++) {
		paramExpr.push_back((*i)->getExpr(outError));

		if (outError.errType != ErrorType::NULLERROR) {
			return nullptr;
		}
	}

	return new RunFunc(token.value, paramExpr);
}