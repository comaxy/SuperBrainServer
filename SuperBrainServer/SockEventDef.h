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

	// 在线玩家列表请求
	GET_PLAYER_LIST_REQUEST = 5,

	// 在线玩家列表响应
	// BODY格式为 playerName1;playerName2;...
	GET_PLAYER_LIST_RESPONSE = 6,

	// BODY: "playerName;gameName"
	CHALLENGE_FRIEND_REQUEST = 7,

	// BODY: "FLAG;REASON"
	// FLAG: 1 同意 2 不同意 3 无法找到朋友
	CHALLENGE_FRIEND_RESPONSE = 8,

	// 100 - 200 for rapid calculation
	RAPID_CALCULATION_BEGIN = 100,

	// BODY: Number1 Number2 
	RC_START = 101,

	// BODY: "Result;Time"
	RC_RESULT = 102,

	// BODY: "Name;Result;Time;Name;Result;Time;RightResult"
	RC_FINAL = 103,

	RAPID_CALCULATION_END = 200,
};

static const CString RAPID_CALCULATION = TEXT("RapidCalculation");
