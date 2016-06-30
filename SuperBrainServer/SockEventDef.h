#pragma once

/*
 * 基本结构:
 * 第一个字节为事件ID
 * 第二个字节和第三个字节合起来为BODY的长度
 * 前三个字节合称为为HEADER
 * 第四个字节开始为BODY
*/

enum SockEvent
{
	// 注册，BODY为UTF8字符串，格式为 "用户名;密码"
	REGISTER = 1,

	// 注册结果，BODY为UTF8字符串，格式为 "FLAG;REASON"
	// FLAG可为 1:表示注册成功 2:表示注册失败
	// REASON在成功时无意义，在失败时表示失败原因
	REG_RESULT = 2,

	// 登录，BODY为UTF8字符串，格式为 "用户名;密码"
	LOGIN = 3,

	// 登录结果，BODY为UTF8字符串，格式为 "FLAG;REASON"
	// FLAG 1:表示成功 2:表示失败
	// REASON 成功时无意义，失败是表示失败原因
	LOGIN_RESULT = 4,
};
