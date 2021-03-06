/***************************************************/
/*				CmdOperator库					   */
/*          版本v1.0.0.00007					   */
/*  该库使用MIT License，详见LICENSE文件			*/
/*            作者Zimo J.						   */
/***************************************************/
//CmdOperator类


#include <map>
#include <string>
#include <functional>
#include <array>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <queue>
#include <sstream>

constexpr auto EXEC_FAILURE = -6210;
constexpr auto DEFAULT_HISTORYSIZE = 1000;


class CmdOperator {
public:
	//参数列表类型的定义
	typedef std::array<std::string, 100> argListType;

	typedef std::function<int(CmdOperator *, const argListType)> commandHandler;

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

	//历史命令列表
	//搭配指向history队列的开头的queueBegin迭代器
	//该列表选择vector作为容器类型是因为vector可以让用户更为灵活的调整列表大小
	std::vector<std::string> history;
	std::vector<std::string>::iterator queueBegin;

	//历史命令的数量是否大于history的大小
	//若此变量为false，则没有超过，队列尾部为history的开头元素
	//若此变量为true，则超过了队列大小，队列尾部为queueBegin+1
	//若queueBegin指向最后一个元素，则队列尾部为history的开头
	bool isMoreThanSize;

	//用于添加历史记录的函数，此操作对于用户应该是不可见的
	void addHistory(const std::string &historyCommand) {
		//queueBegin为尾后迭代器（当且仅当没有一条历史记录储存于history中时）
		if (queueBegin == history.end()) {
			//则让queueBegin指向vector头
			queueBegin = history.begin();
		}
		//queueBegin指向最后一个元素
		else if (queueBegin == history.end() - 1) {
			//让queueBegin指向vector头
			queueBegin = history.begin();
			//并且由于历史记录比history的容量多，将isMoreThanSize设置为true
			isMoreThanSize = true;
		}
		else {
			++queueBegin;
		}
		*queueBegin = historyCommand;
	}

public:
	//默认构造函数
	CmdOperator() :
		op(">"), sheet({}), environmentValue({}), isExit(false), errorHandler(nullptr), \
		history(DEFAULT_HISTORYSIZE), queueBegin(), isMoreThanSize(false)
	{
		queueBegin = history.end();//尾后迭代器
	}

	CmdOperator(const std::string &_op, const std::function<int(const std::string&)> _eh = nullptr,
		const std::size_t _size=DEFAULT_HISTORYSIZE, const std::map<std::string, commandHandler> _sheet = {}) :
		op(_op), sheet(_sheet), environmentValue({}), isExit(false), errorHandler(_eh),\
		history(_size), queueBegin(), isMoreThanSize(false)
	{
		queueBegin = history.end();
	};

	~CmdOperator() {};

	//禁止进行拷贝操作
	CmdOperator(const CmdOperator &co) = delete;

	//禁止进行赋值操作
	CmdOperator &operator=(const CmdOperator &co) = delete;

	//获取环境变量的值
	//**注意，该函数可能会抛出std::out_of_range异常
	std::string getEnv(const std::string &name) {
		return environmentValue.at(name);//at函数可能抛出out_of_range异常（C++11）
	}

	//设置环境变量
	std::string setEnv(const std::string &name, const std::string &value) {
		if (name.substr(0, 1) == "__") {//若开头两个字符为__
			throw std::invalid_argument("Error:将要对CmdOperator系统只读变量进行写操作");
			return nullptr;
		}
		return environmentValue[name] = value;//写入
	}

	//获取历史记录
	//注意，index从一开始编号，数值越大代表越旧的历史记录
	std::string getHistory(const unsigned int index) {
		if (index == 0 || index > history.size())//若index大于历史记录大小，则越界，若index为0则是无效索引
			throw std::out_of_range("无效的历史记录索引值");
		if (!isMoreThanSize) {
			unsigned int size = queueBegin - history.begin() + 1;//当前队列大小
			if (size < index)
				throw std::out_of_range("无效的历史记录索引值");
			else {
				return *(queueBegin - (index - 1));
			}
		}
		return *(queueBegin - (index - 1));
	}

	//编辑一条命令注册项
	//若给定的str命令不存在，则新建该命令的注册项
	//若存在，则更改str命令的处理函数为func
	commandHandler editCommand(const std::string &str, const commandHandler func);

	//通过调用上一个版本来完成操作
	commandHandler editCommand(const std::pair< const std::string, const commandHandler> p) {
		return editCommand(p.first, p.second);
	}

	//从给定的列表中获取命令参数来更改命令的注册
	const std::map<const std::string, const commandHandler> editCommand(const std::map<const std::string, const commandHandler> list) {
		for (const auto &elem : list) {
			sheet[elem.first] = elem.second;
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
	int execute(std::ostream &out, std::istream &in);
};

//执行命令行
//out:输出流
//in:输入流
int CmdOperator::execute(std::ostream & out, std::istream & in) {
	std::string arg, command;
	argListType argList = {};//默认为空
	while (!isExit) {//不退出
		out << op;//输出提示符
		in >> command;
		if (command == "")//若命令为空，则重新开始循环
			continue;
		std::getline(in, arg);
		this->addHistory(arg);
		std::istringstream strReader(arg);
		std::string tmp;
		//准备向commandHandler中提供用户参数（提供的参数中不含命令名称）
		for (auto beg = argList.begin(); strReader >> tmp; ++beg) {
			if (beg == argList.end()) {//当用户的参数超过了array大小时就会迭代至尾后元素
				throw std::length_error("用户提供的参数过多");
				return EXEC_FAILURE;
			}
			*beg = tmp;
		}
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

//编辑命令注册项
CmdOperator::commandHandler CmdOperator::editCommand(const std::string& str, const CmdOperator::commandHandler func)
{
	this->sheet[str] = func;
	return func;
}

