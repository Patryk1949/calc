#include "calculate.hpp"


std::vector<FuncRunInfo> parseTokens(const Token *input);
double recCalculate(const std::vector<FuncRunInfo> &functions, RunInfo &runInfo);

size_t parse0(const Token **tokenI, FuncParseInfo *parseInfo);
size_t parse1(const Token **tokenI, FuncParseInfo *parseInfo);
size_t parse2(const Token **tokenI, FuncParseInfo *parseInfo);
size_t parse3(const Token **tokenI, FuncParseInfo *parseInfo);
size_t parsePostfix(const Token **tokenI, FuncParseInfo *info, size_t rootNodeIndex);
size_t parsePastComma(const Token **tokenI, FuncParseInfo *info, size_t exprSize);
size_t parse4(const Token **tokenI, FuncParseInfo *parseInfo);
size_t parse5(const Token **tokenI, FuncParseInfo *parseInfo);
size_t parse6(const Token **tokenI, FuncParseInfo *parseInfo);

void parseFunction(const Token **tokenI, FuncParseInfo *parseInfo);



int main(int argc, char **argv){
	
	std::vector<Token> tokens;

	if (argc != 2) raiseError("wrong argument count");

	if (!strncmp(argv[1], "-f", 2)){

		const bool printFile = argv[1][2]=='p' ? true :
			[x=argv[1][2]](){ if (x){ puts("ERROR: wrong fomat argument"); exit(1); } else return false; }();


		FILE *const inputFile = fopen(argv[2], "r");
		if (!inputFile){
			puts("ERROR: this file does not exist");
			return 1;
		}
		fseek(inputFile, 0, SEEK_END);
   	const size_t fileSize = ftell(inputFile);

   	char *const str = (char *)malloc((fileSize + 1)*sizeof(char));

   	rewind(inputFile);
   	fread(str, sizeof(char), fileSize, inputFile);
		
		if (printFile) printf("%s ", str);
		fclose(inputFile);
		
		str[fileSize] = '\0';
		tokens = makeTokens(str);
	} else tokens = makeTokens(argv[1]);

	std::vector<FuncRunInfo> functions = parseTokens(&*std::begin(tokens));
	RunInfo runInfo;
	runInfo.variables.resize(functions.front().varsCount);

	printf("= %lf\n", recCalculate(functions, runInfo));

	return 0;
}








size_t parse0(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;
	const size_t currNodeIndex = std::size(*nodes);

	switch (iter->type){
	case Token::Type::Value:
		nodes->emplace_back(Token::Type::Value, iter->value);
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	case Token::Type::Variable:{
			const char *const varName = std::begin(iter->name);
			for (size_t i=std::size(info->varNames); i; --i)
				if (!strcmp(std::begin(info->varNames[i-1]), varName)){
					++iter;
					if (i > info->funcs.front().varsCount)
						nodes->emplace_back(Token::Type::LocalVariable, i-info->funcs.front().varsCount-1);
					else
						nodes->emplace_back(Token::Type::Variable, i-1);
					return parsePostfix(tokenI, info, std::size(*nodes)-1);
				}
			if (iter[1].type == Token::Type::ParenL){
				for (size_t i=0; i!=std::size(info->funcNames); ++i)
					if (!strcmp(std::begin(info->funcNames[i].name), varName)){
						iter += 2;
						size_t argsCount = info->funcNames[i].argsCount;
						
						nodes->emplace_back(Token::Type::Call);
						nodes->back().numbers[0] = i + 1;
						nodes->back().numbers[1] = argsCount;
						size_t exprSize = 1;

						size_t argCounter = 0;
						if (iter->type != Token::Type::ParenR){
							exprSize += parse3(tokenI, info);
							++argCounter;
							
							while (iter->type != Token::Type::ParenR){
								iter += iter->type == Token::Type::Comma;
								exprSize += parse3(tokenI, info);
								++argCounter;
							}
						} ++iter;
						if (argCounter != argsCount) raiseError("wrong number of arguments to function: ", varName, "");
						
						return parsePostfix(tokenI, info, std::size(*nodes)-exprSize);
					}
			}
			raiseError("variable \'", std::begin(iter->name), "\' was not declared");
		}
	case Token::Type::ParenL: ++iter;{
		parse3(tokenI, info);
		if (iter->type != Token::Type::ParenR)
			raiseError("pharenthesis were opened, but not closed");
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	}
	case Token::Type::Abs: ++iter;{
		nodes->emplace_back(Token::Type::Abs);
		parse3(tokenI, info);
		if (iter->type != Token::Type::Abs)
			raiseError("absolute value symbol was opened, but not closed");
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	}
	case Token::Type::Add: ++iter;
		return parse0(tokenI, info);
	case Token::Type::Subtract: ++iter;
		nodes->emplace_back(Token::Type::UMinus);
		return 1 + parse0(tokenI, info);
	case Token::Type::Pi:
	case Token::Type::Euler:
		nodes->emplace_back(Token::Type::Value, iter->type==Token::Type::Pi ? M_PI : M_E);
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	case Token::Type::Acos:
	case Token::Type::Acosh:
	case Token::Type::Asin:
	case Token::Type::Asinh:
	case Token::Type::Atan:
	case Token::Type::Atanh:
	case Token::Type::Acot:
	case Token::Type::Acoth:
	case Token::Type::Cbrt:
	case Token::Type::Ceil:
	case Token::Type::Cos:
	case Token::Type::Cosh:
	case Token::Type::Erf:
	case Token::Type::Exp:
	case Token::Type::Exp2:
	case Token::Type::Floor:
	case Token::Type::Gamma:
	case Token::Type::Ln:
	case Token::Type::Log10:
	case Token::Type::Log2:
	case Token::Type::Round:
	case Token::Type::Sqrt:
	case Token::Type::Step:
	case Token::Type::Sin:
	case Token::Type::Sinh:
	case Token::Type::Sign:
	case Token::Type::Tan:
	case Token::Type::Tanh:
	case Token::Type::Cot:
	case Token::Type::Coth:
		nodes->emplace_back(iter->type);
		++iter;
		if (iter->type == Token::Type::ParenL){
			++iter;
			parse3(tokenI, info);
			if (iter->type != Token::Type::ParenR)
				raiseError("pharenthesis were opened, but not closed");
			++iter;
			return parsePostfix(tokenI, info, currNodeIndex);
		}
		return 1 + parse0(tokenI, info);
	case Token::Type::Atan2:
	case Token::Type::Log:
	case Token::Type::Mod:
		nodes->emplace_back(iter->type);
		++iter;
		if (iter->type == Token::Type::ParenL){
			++iter;
			parse3(tokenI, info);
			iter += iter->type == Token::Type::Comma;
			parse3(tokenI, info);
			if (iter->type != Token::Type::ParenR)
				raiseError("pharenthesis were opened, but not closed");
			++iter;
			return parsePostfix(tokenI, info, currNodeIndex);
		}
		{
			const size_t s = 1 + parse0(tokenI, info);
			return s + parse0(tokenI, info);
		}

	case Token::Type::Sum:
	case Token::Type::Product:
	case Token::Type::Integral:{
		const Token::Type operationType = iter->type;
		++iter;
		constexpr const char *const options[3] = {"sum", "product", "integral"};
		if (iter->type != Token::Type::Variable)
			raiseError("expected variable definition inside ", options[(uint32_t)operationType-(uint32_t)Token::Type::Sum], " expression");
		
		const char *const indexVarName = std::begin(iter->name);
		if (iter[1].type != Token::Type::Assign)
			raiseError("non initialized variable inside ", options[(uint32_t)operationType-(uint32_t)Token::Type::Sum], "expressin");

		iter += 2 + (iter->type == Token::Type::Comma);

		nodes->emplace_back(operationType, info->funcs[info->funcIndex].varsCount);
		++info->funcs[info->funcIndex].varsCount;
		size_t exprSize = 1 + parse3(tokenI, info);
		exprSize += parse3(tokenI, info);

		if (iter->type != Token::Type::BracketR)
			raiseError("missing closing square bracket inside", options[(uint32_t)operationType-(uint32_t)Token::Type::Sum], "expression");
		++iter;

		info->varNames.emplace_back();
		strcpy(std::begin(info->varNames.back()), indexVarName);
		exprSize += parse3(tokenI, info);
		info->varNames.back()[0] = '\0';
		return exprSize;
	}
	default:
		raiseError("wrong syntax");
	}
}


size_t parse1(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse0(tokenI, info);
	
	while (iter->type == Token::Type::Power){
		++iter;
		nodes->emplace_back();
		std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
		new(&*nodes->end()-exprSize-1) Token{Token::Type::Power};
		exprSize += 1 + parse0(tokenI, info);
	}
	return exprSize;
}

size_t parse2(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse1(tokenI, info);
	{
		Token::Type tokType = iter->type;
		while (tokType==Token::Type::Multiply || tokType==Token::Type::Divide){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse1(tokenI, info);
			tokType = iter->type;
		}
	} return exprSize;
}

size_t parse3(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	const size_t exprStart = std::size(*nodes); 
	size_t exprSize = parse2(tokenI, info);
	{
		Token::Type tokType = iter->type;
		// if (tokType == Token::Type::Abs){
		// 	++iter;
		// 	const size_t exprEnd = exprStart + exprSize;
		// 	nodes->emplace_back();
		// 	exprSize += parse6(tokenI, info);
		// 	std::rotate(std::begin(*nodes)+exprStart, std::begin(*nodes), std::end(*nodes));
		// 	return exprSize;
		//	}
		while (tokType==Token::Type::Add || tokType==Token::Type::Subtract){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse2(tokenI, info);
			tokType = iter->type;
		}
	} return exprSize;
}

size_t parsePostfix(const Token **const tokenI, FuncParseInfo *const info, const size_t rootIndex){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;
	while (iter->type == Token::Type::Factorial){
		++iter;
		nodes->emplace_back();
		std::move_backward(std::begin(*nodes)+rootIndex, std::end(*nodes)-1, std::end(*nodes));
		new(&*std::begin(*nodes)+rootIndex) Token{Token::Type::Factorial};
	}
	return std::size(*nodes) - rootIndex;
}

size_t parsePastComma(const Token **const tokenI, FuncParseInfo *const info, size_t exprSize){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;
	{
		Token::Type tokType = iter->type;
		while (tokType==Token::Type::Add || tokType==Token::Type::Subtract){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse2(tokenI, info);
			tokType = iter->type;
		}
	} return exprSize;
}

size_t parse6(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse5(tokenI, info);
	
	while (iter->type == Token::Type::Or){
		++iter;
		nodes->emplace_back();
		std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
		new(&*nodes->end()-exprSize-1) Token{Token::Type::Or};
		exprSize += 1 + parse5(tokenI, info);
	}
	return exprSize;
}

size_t parse5(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse4(tokenI, info);
	
	while (iter->type == Token::Type::And){
		++iter;
		nodes->emplace_back();
		std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
		new(&*nodes->end()-exprSize-1) Token{Token::Type::And};
		exprSize += 1 + parse4(tokenI, info);
	}
	return exprSize;
}

size_t parse4(const Token **const tokenI, FuncParseInfo *const info){
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse3(tokenI, info);
	{
		Token::Type tokType = iter->type;
		while (tokType>=Token::Type::Assign || tokType<=Token::Type::GreaterEq){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse3(tokenI, info);
			tokType = iter->type;
		}
	} return exprSize;
}


std::vector<FuncRunInfo> parseTokens(const Token *input){	// VarValues should be array of pointers to variable values stored in nodes
	const Token *iter = input;
	FuncParseInfo funcInfo;
	funcInfo.funcs.emplace_back();

	for (;;){
		if (iter[0].type==Token::Type::Variable && iter[1].type==Token::Type::Assign){
			const char *const varName = std::begin(iter->name);
			iter += 2;
			if (iter->type == Token::Type::ParenL){
				++iter;
				funcInfo.funcNames.emplace_back();
				strcpy(std::begin(funcInfo.funcNames.back().name), varName);
				parseFunction(&iter, &funcInfo);
			} else{
				for (auto I=std::begin(funcInfo.varNames); I!=std::end(funcInfo.varNames); ++I)
					if (!strcmp(std::begin(*I), varName)) raiseError("redefinition of variable: ", varName, "");
				
				funcInfo.funcs.front().code.emplace_back(Token::Type::VariableDefinition, funcInfo.funcs.front().varsCount);
				++funcInfo.funcs.front().varsCount;
				funcInfo.varNames.emplace_back();
				strcpy(std::begin(funcInfo.varNames.back()), varName);
				
				parse3(&iter, &funcInfo);
				iter += iter->type == Token::Type::Comma;
			}
		} else if (iter->type == Token::Type::Plot){
			++iter;
			if (iter->type != Token::Type::Variable)
				raiseError("expected variable definition as first plot argument");
			
			const char *const indexVarName = std::begin(iter->name);
			if (iter[1].type != Token::Type::Assign)
				raiseError("plot's argument that defines range is missing");

			iter += 2 + (iter->type == Token::Type::Comma);

			funcInfo.funcs.front().code.emplace_back(Token::Type::PlotAutoRes, funcInfo.funcs.front().varsCount);
			++funcInfo.funcs.front().varsCount;
			size_t exprSize = 1 + parse3(&iter, &funcInfo);
			iter += iter->type == Token::Type::Comma;
			exprSize += parse3(&iter, &funcInfo);
			iter += iter->type == Token::Type::Comma;
			exprSize += parse3(&iter, &funcInfo);
			iter += iter->type == Token::Type::Comma;
			exprSize += parse3(&iter, &funcInfo);

			if (iter->type != Token::Type::BracketR){
				funcInfo.funcs.front().code[std::size(funcInfo.funcs.front().code)-exprSize].type = Token::Type::Plot;
				iter += iter->type == Token::Type::Comma;
				exprSize += parse3(&iter, &funcInfo);
				if (iter->type != Token::Type::BracketR) raiseError("missing closing square bracket inside plot argument list");
			}
			++iter;

			funcInfo.varNames.emplace_back();
			strcpy(std::begin(funcInfo.varNames.back()), indexVarName);
			exprSize += parse3(&iter, &funcInfo);
			funcInfo.varNames.back()[0] = '\0';
		} else{
			if (iter->type == Token::Type::End) raiseError("no returning statement");
			parse3(&iter, &funcInfo);
			if (iter->type != Token::Type::End) raiseError("statement after returning expression in function");
			break;
		}
	}
	return std::move(funcInfo.funcs);
}

void parseFunction(const Token **const tokenI, FuncParseInfo *const info){	// VarValues should be array of pointers to variable values stored in nodes
	const Token *&iter = *tokenI;


	size_t argsCount = 0;
	while (iter->type != Token::Type::ParenR){
		if (iter->type != Token::Type::Variable)
			raiseError("missing argument's name inside definition of function: ", std::begin(info->funcNames.back().name), "");
		for (auto I=std::begin(info->varNames)+info->funcs.front().varsCount; I!=std::end(info->varNames); ++I)
			if (!strcmp(std::begin(*I), std::begin(iter->name))) raiseError("duplicated function's argument ", std::begin(info->funcNames.back().name), "");
		info->varNames.emplace_back();
		strcpy(std::begin(info->varNames.back()), std::begin(iter->name));
		++argsCount;
		++iter;
	}

	if (iter[1].type != Token::Type::BraceL)
		raiseError("missing opening brace in:", std::begin(info->funcNames.back().name), " function definition");
	iter += 2;

	info->funcNames.back().argsCount = argsCount;
	info->funcIndex = std::size(info->funcs);
	info->funcs.emplace_back();
	info->funcs.back().varsCount = argsCount;
	
	for (;;){
		if (iter[0].type==Token::Type::Variable && iter[1].type==Token::Type::Assign){
			const char *const varName = std::begin(iter->name);
			iter += 2;

			if (iter->type == Token::Type::ParenL) raiseError("function defined inside another function");		
			for (auto I=std::begin(info->varNames)+info->funcs.front().varsCount; I!=std::end(info->varNames); ++I)
				if (!strcmp(std::begin(*I), varName)) raiseError("redefinition of variable ", varName, "");
			
			info->funcs.back().code.emplace_back(Token::Type::LocalVariableDefinition, info->funcs.back().varsCount);
			++info->funcs.back().varsCount;
			info->varNames.emplace_back();
			strcpy(std::begin(info->varNames.back()), varName);
			
			parse3(&iter, info);
			iter += iter->type == Token::Type::Comma;
		} else{
			if (iter->type==Token::Type::BraceR) raiseError("no returning statement in function: ", std::begin(info->funcNames.back().name), "");
			parse3(tokenI, info);
			if (iter->type != Token::Type::BraceR) raiseError("statement after returning expression in function:", std::begin(info->funcNames.back().name), "");
			break;
		}
	}
	++iter;
	info->funcIndex = 0;
	info->varNames.resize(info->funcs.front().varsCount);
}