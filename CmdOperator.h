//CmdOperator类

#include "pch.h"
#include <map>
#include <string>
#include <functional>
#include <list>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <sstream>


class CmdOperator {
public:
	typedef std::function<int(CmdOperator *, const std::list<std::string>)> commandHandler;

	typedef const std::list<std::string> argListType;

private:
	std::string op;//提示符

	//建立命令与函数的对应调用关系，
	//其中索引为命令，函数指针指向一个接受初始值列表（类型为list）的函数，该函数返回一个表示状态的int值
	//函数接受的初始值列表为命令后的字符串
	//如command arg1 arg2,会调用sheet["command"]({"arg1","arg2"})
	std::map<std::string, commandHandler> sheet;

	//环境变量
	//索引为变量名，值为变量值
	//若变量名由__（双下划线）开头，则该变量为系统变量，拥有只读属性，如__result
	std::map<std::string, std::string> environmentValue;

	//是否要退出
	//若置位则在执行下一次命令行循环时退出
	bool isExit;

	//当出现错误的命令时如何处理
	std::function<int(const std::string&)> errorHandler;

public:
	//默认构造函数
	CmdOperator() :
		op(">"), sheet({}), environmentValue({}), isExit(false), errorHandler(nullptr) {};

	CmdOperator(const std::string &_op, const std::function<int(const std::string&)> _eh = nullptr,
		const std::map<std::string, commandHandler> _sheet = {}) :
		op(_op), sheet(_sheet), environmentValue({}), isExit(false), errorHandler(_eh) {};

	~CmdOperator() {};

	CmdOperator(const CmdOperator &co) = delete;

	CmdOperator &operator=(const CmdOperator &co) = delete;

	//获取环境变量的值
	//**注意，该函数可能会抛出std::out_of_range异常
	std::string getEnv(const std::string &name) {
		return environmentValue.at(name);//at函数可能抛出out_of_range异常（C++11）
	}

	//设置环境变量
	std::string setEnv(const std::string &name, const std::string &value) {
		if (name.substr(0, 1) == "__") {//若开头两个字符为__
			throw std::invalid_argument("Error:将要对系统只读变量进行写操作");
			return nullptr;
		}
		return environmentValue[name] = value;//写入
	}

	//添加一条命令
	//**注意，该函数会抛出std::invalid_argument异常
	commandHandler addCommand(const std::string &str, const commandHandler func) {
		try {
			sheet.at(str);//首先使用at尝试是否存在名为str的命令
		}
		catch (std::out_of_range &e) {
			return sheet[str] = func;//若非，则进行添加操作
		}
		throw std::invalid_argument(str+"是已存在的命令");
	}

	//通过调用上一个版本来完成操作
	commandHandler addCommand(const std::pair< const std::string, const commandHandler> p) {
		return addCommand(p.first, p.second);
	}

	//从给定的列表中获取命令参数
	const std::map<const std::string, const commandHandler> addCommand(const std::map<const std::string, const commandHandler> list) {
		for (const auto &elem : list) {
			sheet[elem.first] = elem.second;
		}
		return list;
	}

	commandHandler replaceCommand(const std::string &index, const commandHandler newFunc) {
		//此处与addCommand相似，但只当存在index所对应的函数时替换
		try {
			return sheet.at(index) = newFunc;
		}
		catch (std::out_of_range &e) {
			throw std::invalid_argument(index + "无效的索引名");
		}
	}

	commandHandler replaceCommand(const std::pair<const std::string, const commandHandler> p) {
		return replaceCommand(p.first, p.second);
	}

	std::map<const std::string, const commandHandler> replaceCommand(const std::map<const std::string, const commandHandler> list) {
		for (const auto elem : list) {
			replaceCommand(elem);
		}
		return list;
	}

	//退出该命令行
	int exitCommandLine() {
		isExit = true;
		return std::stoi(environmentValue["__result"]);
	}

	//设置错误处理函数
	std::function <int(const std::string&)> setErrHandle(const std::function<int(const std::string&)> func) {
		return errorHandler = func;
	}

	//删除指定命令
	//若存在并成功被删除，则返回1
	//若否，则返回0
	int removeCommand(const std::string &index) {
		return sheet.erase(index);
	}

	//执行命令行
	//out:输出流
	//in:输入流
	int execute(std::ostream &out, std::istream &in) {
		std::string arg, command;
		std::list<std::string> argList = {};//默认为空
		while (!isExit) {//不退出
			out << op;//输出提示符
			in >> command;
			if (command == "")//若命令为空，则重新开始循环
				continue;
			std::getline(in, arg);
			std::istringstream strReader(arg);
			std::string tmp;
			while (strReader >> tmp)
				argList.push_back(tmp);
			auto matchFunc = sheet.find(command);
			if (matchFunc == sheet.end()) {
				if (errorHandler != nullptr)
					errorHandler(command);//调用错误处理函数
				else {
					out << "错误的命令" << command << std::endl;
					environmentValue["__result"] = "-1";
				}
				continue;//重新开始循环
			}
			else {
				//执行目标函数，返回值储存于__result中
				environmentValue["__result"] = std::to_string(matchFunc->second(this, argList));
			}
		}
		return std::stoi(environmentValue["__result"]);
	}
};
