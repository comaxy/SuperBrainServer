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
};
