


//����������ļ��ٶȣ�������ӵ�xp����
//��ȫ����������
float acc_corect(float dt,float& ap,float& vp,float& xp,float& ae,float& ve,float& xm,float& k)
{
	
	xp += (vp-ve)*dt;
	float dpm = xp - xm;	//xe + R
	ve += k*dpm/dt;			//�����´�����xp
	ae += ve/dt;
	
	float ar = ap -ae;
	vp += ar*dt;
	
	float ar;
}