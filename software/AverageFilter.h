#ifndef _AVERAGEFILTER_H_
#define _AVERAGEFILTER_H_
#include "sys.h"



//��ֵ�˲�,���������˲�
template <typename T, uint8_t _size>
class AverageFilter
{
public:
	AverageFilter()
	{
		reset();
	}

	void reset();
	const T& apply(const T& v);
private:
	uint8_t _count;
	T _last_output;
	T _buffer[_size];
};





template <typename T, uint8_t _size>
void AverageFilter<T,_size>::reset()
{
	_last_output = 0;
	_count = 0;
	for (uint8_t i = 0; i < _size; i++)
	{
		_buffer[i] = 0.0f;
	}
}

template <typename T, uint8_t _size>
const T& AverageFilter<T,_size>::apply(const T& v)
{
	_buffer[_count] = v;		//��������

	uint8_t oldest = _count + 1;		//�����һ����������
	if (oldest == _size)oldest = 0;

	if (++_count == _size)_count = 0;		//ָ����һ��

	_last_output = _last_output + (v - _buffer[oldest]) / _size;
	return _last_output;
}




#endif

