#pragma once

/*
 * �����ṹ:
 * ��һ���ֽ�Ϊ�¼�ID
 * �ڶ����ֽں͵������ֽں�����ΪBODY�ĳ���
 * ǰ�����ֽںϳ�ΪΪHEADER
 * ���ĸ��ֽڿ�ʼΪBODY
*/

enum SockEvent
{
	// ע�ᣬBODYΪUTF8�ַ�������ʽΪ "�û���;����"
	REGISTER = 1,

	// ע������BODYΪUTF8�ַ�������ʽΪ "FLAG;REASON"
	// FLAG��Ϊ 1:��ʾע��ɹ� 2:��ʾע��ʧ��
	// REASON�ڳɹ�ʱ�����壬��ʧ��ʱ��ʾʧ��ԭ��
	REG_RESULT = 2,

	// ��¼��BODYΪUTF8�ַ�������ʽΪ "�û���;����"
	LOGIN = 3,

	// ��¼�����BODYΪUTF8�ַ�������ʽΪ "FLAG;REASON"
	// FLAG 1:��ʾ�ɹ� 2:��ʾʧ��
	// REASON �ɹ�ʱ�����壬ʧ���Ǳ�ʾʧ��ԭ��
	LOGIN_RESULT = 4,

	// ��������б�����
	GET_PLAYER_LIST_REQUEST = 5,

	// ��������б���Ӧ
	// BODY��ʽΪ playerName1;playerName2;...
	GET_PLAYER_LIST_RESPONSE = 6,

	// BODY: "playerName;gameName"
	CHALLENGE_FRIEND_REQUEST = 7,

	// BODY: "FLAG;REASON"
	// FLAG: 1 ͬ�� 2 ��ͬ�� 3 �޷��ҵ�����
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
