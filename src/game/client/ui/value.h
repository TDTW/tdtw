#ifndef GAME_CLIENT_UI_VALUE_H
#define GAME_CLIENT_UI_VALUE_H

#include <base/vmath.h>

enum ANIMATION_TYPE
{
	LINEAR,
	EaseIN,				// ������, ������� ��������, ������ ������, ����� �������� - ����� ��������
	EaseOUT,			// ������, ������� ������, ������ ��������, ����� �������� - ����� ��������
	EaseINOUT,			// ������, ������� � ����� ��������, ����� �������� - ����� ��������
	EaseIN2,			// ������, ������� ��������, ������ ������, ����� �������� - ��������
	EaseOUT2,			// ������, ������� ������, ������ ��������, ����� �������� - ��������
	EaseINOUT2,			// ������, ������� � ����� ��������, ����� �������� - ��������
	EaseIN3,			// ������, ������� ��������, ������ ������, ����� �������� - ������
	EaseOUT3,			// ������, ������� ������, ������ ��������, ����� �������� - ������
	EaseINOUT3,			// ������, ������� � ����� ��������, ����� �������� - ������
	EaseIN4,			// ������, ������� ��������, ������ ������, ����� �������� - ������
	EaseOUT4,			// ������, ������� ������, ������ ��������, ����� �������� - ������
	EaseINOUT4,			// ������, ������� � ����� ��������, ����� �������� - ������
	EaseIN5,			// ������, ������� ��������, ������ ������, ����� �������� - ����� ������
	EaseOUT5,			// ������, ������� ������, ������ ��������, ����� �������� - ����� ������
	EaseINOUT5,			// ������, ������� � ����� ��������, ����� �������� - ����� ������
	EaseIN6,            // ������, ������� ��������, ������ ������, ����� �������� - ����� ����� ������
	EaseOUT6,            // ������, ������� ������, ������ ��������, ����� �������� - ����� ����� ������
	EaseINOUT6,            // ������, ������� � ����� ��������, ����� �������� - ����� ����� ������
	EaseINCirc,            // ������, ������� ��������, ������ ������, ����� �������� - ����� ����� ������
	EaseOUTCirc,        // ������, ������� ������, ������ ��������, ����� �������� - ������ ������� ����������
	EaseINOUTCirc,        // ������, ������� � ����� ��������, ����� �������� - ������ ������� ����������
	EaseINBack,			// ������ c �����������, ������� �����������
	EaseOUTBack,		// ������ c �����������, ������ �����������
	EaseINOUTBack,		// ������ c �����������, ������� � ������ �����������
	EaseINElastic,		// ������, ������ ���������
	EaseOUTElastic,		// ������, ������� ���������
	EaseINOUTElastic,	// ������, � ������ ���������
	EaseINBounce,		// ������, ������� ��������
	EaseOUTBounce,		// ������, ����� �������
	EaseINOUTBounce,	// ������, ������� � ������� � ������
	Default,
};
class CValue
{
public:
	CValue();
	CValue(class CNUIElements *pNUI);
	void Init(vec4 value);
	void Init(vec4 value, float time, ANIMATION_TYPE animation_type);

	void InitPlus(vec4 value, float time, ANIMATION_TYPE animation_type);

	void Recalculate();
	void EndAnimation();

	bool m_AnimEnded;
	double m_AnimTime;
	double m_AnimEndTime;
	ANIMATION_TYPE m_Animation;
	vec4 m_Value, m_NewValue, m_OldValue;

private:
	class CNUIElements *m_pNui;

	vec4 Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, float time);

	float BounceOut(float time);
};

#endif //GAME_CLIENT_UI_VALUE_H
