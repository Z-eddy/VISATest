#include <iostream>
#include<string>
#include "visa.h"
using std::cerr;
using std::cout;
using std::endl;
using std::ends;
using std::string;

static ViSession defaultRM;//resource manager
static ViSession instr;
static ViStatus status;

void close() {
	viClose(defaultRM);
	viClose(instr);
	//fflush(stdin);
	//getchar();
}

int main()
{
	status = viOpenDefaultRM(&defaultRM);//获得manager 句柄
	if (status < VI_SUCCESS) {
		cerr << "无法打开manager句柄" << endl;
		exit(EXIT_FAILURE);
	}

	status = viOpen(defaultRM, "ASRL2::INSTR", VI_NULL, VI_NULL, &instr);//获得设备句柄
	if (status < VI_SUCCESS) {
		cerr << "无法打开设备句柄" << endl;
		close();
		return 0;
	}

	//设置属性
	status = viSetAttribute(instr, VI_ATTR_TMO_VALUE, 5000);//5秒钟超时
	status = viSetAttribute(instr, VI_ATTR_ASRL_BAUD, 9600);//9600波特率
	status = viSetAttribute(instr, VI_ATTR_ASRL_DATA_BITS, 8);//设置数据位为8 bit每帧//一个字节
	status = viSetAttribute(instr, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_ONE);//停止位是-1
	status = viSetAttribute(instr, VI_ATTR_TERMCHAR_EN, VI_TRUE);//设置停止字符有效,当收到停止符时停止
	status = viSetAttribute(instr, VI_ATTR_TERMCHAR, 0xA);//设定停止字符为数字10,对应的字符为\n;

	//写入
	char buffer[512];
	strcpy_s(buffer, "*IDN\n");
	ViUInt32 charCount{ 0 };
	status = viWrite(instr, (ViBuf)buffer, (ViUInt32)strlen(buffer), &charCount);//写入命令
	if (status < VI_SUCCESS) {
		cerr << "写入错误" << endl;
		close();
		return 0;
	}
	cout << "写入字符数:" << charCount << ends << "内容:" << buffer << endl;

	//读取
	status = viRead(instr, (ViBuf)buffer, (ViUInt32)strlen(buffer), &charCount);
	if (status < VI_SUCCESS) {
		cerr << "读取错误" << endl;
		close();
		return 0;
	}
	cout << "读取字符数:" << charCount << ends << "内容:" << buffer << endl;

	close();
	return 0;
}