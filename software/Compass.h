#ifndef _COMPASS_H_
#define _COMPASS_H_
#include "sys.h"
#include "QMath.h"


//��λuT
class Compass
{
public:	
	Compass() :
		_healthy(false),
		_available(false),
		_have_calibration(false),
		_board_orientation(ROTATION_NONE)
	{

	}

	//���´���������
	virtual void update() = 0;		
	//��ʼ��
	virtual void init(void) {};
	//����Ƿ�У׼
	void check_calibration(void);
	bool calibrated() {
		return _have_calibration;
	}
	//�������Ƿ����
	bool healthy(void)const { return _healthy; }
	//���÷���
	void set_board_orientation(Rotation orientation) {
		_board_orientation = orientation;
	}

	bool get_available(void)const { return _available; }
	void set_available(bool v) { _available = v; }
	const Vector3f& get_mag(void)const{
		return _mag;
	}

protected:
	//��ȡ���������ݺ���Ҫ�����˺���У׼
	void _rotate_and_offset(const Vector3f& mag);
	bool _healthy;
	bool _available;
private:

	
	bool _have_calibration;
	Rotation _board_orientation;

	Vector3f _mag;

};

#endif


