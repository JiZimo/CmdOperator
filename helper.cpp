/***************************************************/
/*				CmdOperator库					   */
/*          版本v1.0.0.00007					   */
/*  该库使用MIT License，详见LICENSE文件			*/
/*            作者Zimo J.						   */
/***************************************************/
// helper.cpp 该程序是CmdOperator的帮助程序，同时也是CmdOperator的一个小示例程序

#include "pch.h"
#include "CmdOperator.h"
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

const std::vector<std::string> manualIndex({
	"constructor","getEnv","setEnv","getHistory",
	"addCommand","replaceCommand","exitCommandLine"
	"removeCommand","execute","argListType","commandHandler",
	"setErrHandler","editCommand",
	});

const std::vector<std::string> commandList({
	"?","help","detail","example","sysEnv","Usage","exit"
	});
/*
 *打印给定的函数的简要帮助
 *在CmdOperator中，注册的函数应当接受一个CmdOperator*类型的参数（指向执行该函数的CmdOperator）
 *以及一个CmdOperator::argListType类型的argList
 *关于CmdOperator::argListType的更多信息可以在该帮助程序中输入help argListType以获取
 *关于注册函数类型的信息可以键入help commandHandler以获取
 */
int getHelp(CmdOperator *cmd, CmdOperator::argListType argList);

/*
 *打印某一命令的参数
 */
int getCommandHelp(CmdOperator *cmd, CmdOperator::argListType argList);

/*
 *打印某一手册页的详细信息
 */
int getDetail(CmdOperator *cmd, CmdOperator::argListType argList);

/*
 *打印示例
 */
int getExample(CmdOperator *cmd, CmdOperator::argListType argList);

/*
 *输出某一文件
 */
void outputFile(std::ostream& os, std::fstream& file);

/*
 *检查命令的参数
 */
int checkArg(const std::string& commandName, CmdOperator::argListType arg, const std::map<std::string, std::string> &_map);

int main()
{
	//一般情况下，只需要提供cmd的命令提示符即可
	CmdOperator cmd("[键入?以获取帮助]");
	/*
	 *注册函数操作
	 *左边为命令名，右边为对应着此参数的commandHandler参数
	 */
	cmd.editCommand(commandList[0], getCommandHelp);
	cmd.editCommand(commandList[1], getHelp);
	cmd.editCommand(commandList[2], getDetail);
	cmd.editCommand(commandList[3], getExample);
	//也可以使用lambda注册命令
	cmd.editCommand(commandList[4],[](CmdOperator *cmd,CmdOperator::argListType argList)->int{
		std::fstream file("./DocText/sysEnv.txt");
		outputFile(std::cout,file);
		return 0;
	});
	cmd.editCommand(commandList[5],[](CmdOperator *cmd,CmdOperator::argListType argList)->int{
		std::fstream file("./DocText/usage.txt");
		outputFile(std::cout,file);
		return 0;
	});
	cmd.editCommand(commandList[6], [](CmdOperator * cmd, CmdOperator::argListType argList)->int {
		return cmd->exitCommandLine();
	});
	cmd.execute(std::cout, std::cin);
	return std::stoi(cmd.getEnv("__result"));
}

int getHelp(CmdOperator* cmd, CmdOperator::argListType argList)
{
	std::map<std::string, std::string> helpDoc{
	{manualIndex[0],"程序的构造函数，详见detail constructor"},/*构造函数*/
	{manualIndex[1],"获取由参数指定的环境变量值，详见detail getEnv"},/*getEnv*/
	{manualIndex[2],"设置环境变量值，详见detail setEnv"},/*setEnv*/
	{manualIndex[3],"获取历史记录，详见detail getHistory"},/*getHistory*/
	{manualIndex[4],"注册一条命令，该操作已弃用，详见detail editCommand"},/*editCommand*/
	{manualIndex[5],"更改一条已注册的命令，详见detail replaceCommand"},/*replaceCommand*/
	{manualIndex[6],"操控退出命令行，详见detail exitCommandLine"},/*exitCommandLine*/
	{manualIndex[7],"注销一条已注册的命令，详见detail removeCommand"},/*removeCommand*/
	{manualIndex[8],"执行该命令行，详见detail execute"},/*execute*/
	{manualIndex[9],"参数列表类型，详见detail argListType"},/*argListType*/
	{manualIndex[10],"命令处理函数类型，详见detail commandHandler"},/*commandHandler*/
	{manualIndex[11],"设置错误命令处理函数，详见detail setErrHandler"},/*setErrHandler*/
	{manualIndex[12],"编辑命令注册表项，详见detail editCommand"},/*editCommand*/
	{"all","打印该列表"}/*所有手册页*/
	};
	int errCode;
	if (errCode = checkArg(commandList[1], argList, helpDoc))
		return errCode;
	std::cout << argList[0] << " " << helpDoc.at(argList[0]) << std::endl;
	return 0;
}

int getCommandHelp(CmdOperator * cmd, CmdOperator::argListType argList) {
	//手册页与帮助内容的映射
	std::map<std::string, std::string> commandHelp{
	{commandList[0],"获取某一命令的简要帮助，详见detail commandHelp"},/*?*/
	{commandList[1],"获取CmdOperator的某一操作或类型的简要帮助，详见detail help"},/*help*/
	{commandList[2],"获取某一手册页或某一命令的详细信息，详见detail detail"},/*detail*/
	{commandList[3],"获取CmdOperator的某一操作或类型的示例，详见detail example"},/*example*/
	{commandList[4],"获取CmdOperator系统环境变量相关信息列表"},/*SysEnv*/
	{commandList[5],"获取使用CmdOperator库的帮助"},/*Usage*/
	{commandList[6],"退出命令行"},/*exit*/
	{"all","打印该列表"}/*打印所有*/
	};
	//错误码
	int errCode;
	if (errCode = checkArg(commandList[0], argList, commandHelp))
		return errCode;
	std::cout << argList[0] << " " << commandHelp.at(argList[0]) << std::endl;
	return 0;
}

int getDetail(CmdOperator * cmd, CmdOperator::argListType argList)
{
	std::map<std::string, std::string> detail({
	{manualIndex[0],"./DocText/detail/constructors.txt"},/*构造函数*/
	{manualIndex[1],"./DocText/detail/getEnv.txt"},/*getEnv*/
	{manualIndex[2],"./DocText/detail/setEnv.txt"},/*setEnv*/
	{manualIndex[3],"./DocText/detail/getHistory.txt"},/*getHistory*/
	{manualIndex[4],"./DocText/detail/addCommand.txt"},/*addCommand*/
	{manualIndex[5],"./DocText/detail/replaceCommand.txt"},/*replaceCommand*/
	{manualIndex[6],"./DocText/detail/exitCommandLine.txt"},/*exitCommandLine*/
	{manualIndex[7],"./DocText/detail/removeCommand"},/*removeCommand*/
	{manualIndex[8],"./DocText/detail/execute.txt"},/*execute*/
	{manualIndex[9],"./DocText/detail/argListType"},/*argListType*/
	{manualIndex[10],"./DocText/detail/commandHandler.txt"},/*commandHandler*/
	{manualIndex[11],"./DocText/detail/setErrHandler.txt"},/*setErrHandler*/
	{manualIndex[12],"./DocText/detail/editCommand.txt"},/*editCommand*/
	/*下面是helper的命令详细帮助*/
	{commandList[0],"./DocText/detail/commandHelp.txt"},/*?命令*/
	{commandList[1],"./DocText/detail/help.txt"},/*help命令*/
	{commandList[2],"./DocText/detail/detail.txt"},/*detail命令*/
	{commandList[3],"./DocText/detail/example.txt"},/*example命令*/
	});
	std::cout << argList[0] << std::endl;
	std::fstream detailFile;
	//打印文件
	try {
		detailFile.open(detail.at(argList[0]),std::ios::in);
	}
	catch (...) {
		throw;
	}
	outputFile(std::cout, detailFile);
	return 0;
}

int getExample(CmdOperator* cmd, CmdOperator::argListType argList)
{
	std::cout<<"对不起，暂不支持此功能"<<std::endl;
	//std::map<std::string, std::string> example{
	//{manualIndex[0],"./DocText/example/construct.cpp"},/*构造函数*/
	//{manualIndex[1],"./DocText/example/getEnv.cpp"},/*getEnv*/
	//{manualIndex[2],"./DocText/example/setEnv.cpp"},/*setEnv*/
	//{manualIndex[3],"./DocText/example/getHistory.cpp"},/*getHistory*/
	//{manualIndex[4],"./DocText/example/addCommand.cpp"},/*addCommand*/
	//{manualIndex[5],"./DocText/example/replaceCommand.cpp"},/*replaceCommand*/
	//{manualIndex[6],"./DocText/example/exitCommandLine.cpp"},/*exitCommandLine*/
	//{manualIndex[7],"./DocText/example/removeCommand.cpp"},/*removeCommand*/
	//{manualIndex[8],"./DocText/example/execute.cpp"},/*execute*/
	//{manualIndex[9],"./DocText/example/argListType.cpp"},/*argListType*/
	//{manualIndex[10],"./DocText/example/commandHandler.cpp"},/*commandHandler*/
	//{manualIndex[11],"./DocText/example/setErrHandler.cpp"},/*setErrHandler*/
	//};
	//std::cout << argList[0] << std::endl;
	//std::fstream exampleFile;
	//try {
	//	exampleFile.open(example.at(argList[0]), std::ios::in);
	//}
	//catch (...) {
	//	throw;
	//}
	return 0;
}

void outputFile(std::ostream & os, std::fstream &file) {
	std::string str;
	while (std::getline(file, str))//循环直至读取结束
		os << str;
}

int checkArg(const std::string & commandName, CmdOperator::argListType arg, const std::map<std::string, std::string> &_map)
{
	if (arg.size() != 1) {
		std::cerr << commandName << "命令后仅可添加想要阅读的手册页或all参数" << std::endl;
		std::cerr << "请重新输入命令，键入? " << commandName << "可获取关于" << commandName << "的帮助。" << std::endl;
		return 1;
	}
	if (arg[0] == "all") {
		for (auto beg = _map.begin(); beg != _map.end(); ++beg) {
			//打印全部的信息
			std::cout << beg->first << " " << beg->second << std::endl;
		}
	}
	try {
		_map.at(arg[0]);
	}
	catch (std::out_of_range e) {
		std::cout << "非有效手册页或参数：" << arg[0] << std::endl;
		return -1;
	}
	return 0;
}
