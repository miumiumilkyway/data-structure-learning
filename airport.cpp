#include<bits/stdc++.h>
using namespace std;


enum Runway_activity { idle, land, takeoff };	//Enumerates all Runway's states
class Runway {
public:
	Runway(int limit);	//constructor:initialize the Runway
	Error_code can_land(const Plane &current);	//�ж�landing queue�Ƿ��ܼ���ɻ�
	Error_code can_depart(const Plane &current);	//�ж�takeoff queue�Ƿ��ܼ���ɻ�
	Runway_activity activity(int time, Plane &moving);	//�жϵ�ǰ�ܵ����ڸ�ʲô
	void shut_down(int time) const;		//���е�λʱ�������ͨ���ܽ�һ��

private:
	Queue landing;    //  landing queue
	Queue takeoff;     //  takeoff queue
	int queue_limit;	// ���г������ƣ���ɶ��кͽ��������һ����

	//���±���������������ͨ����shut down����
	//  number of planes asking to land
	int num_land_requests;

	//  number of planes asking to take off
	int num_takeoff_requests;

	//  number of planes that have landed
	int num_landings;

	//  number of planes that have taken off
	int num_takeoffs;

	//  number of planes queued to land
	int num_land_accepted;

	//  number of planes queued to take off
	int num_takeoff_accepted;

	//  number of landing planes refused
	int num_land_refused;

	//  number of departing planes refused
	int num_takeoff_refused;

	//  total time of planes waiting to land
	int land_wait;

	//  total time of planes waiting to take off
	int takeoff_wait;

	//  total time runway is idle
	int idle_time;

}

enum Plane_status { null, arriving, departing };	//Enumerates all planes' states

class Plane {
public:
	Plane();	//constructor
	Plane(int flt, int time, Plane_status status);		//constructor
	void refuse() const;	//��ʾĳ�ܷɻ���ĳ����λʱ���ڵ�״̬�����ܾ����/���䣬�����ȴ�
	void land(int time) const;		//��ʾĳ�ܷɻ���ĳ����λʱ���ڵ�״̬�����䣬������������Ҫ��ͨ���������ĳ��Ӳ��������������
	void fly(int time) const;		//��ʾĳ�ܷɻ���ĳ����λʱ���ڵ�״̬����ɣ�������������Ҫ��ͨ���������ĳ��Ӳ��������������
	int started() const;	//���طɻ���ɵ�ʱ�䣬Ҳ����clock_start

private:
	int flt_num;	//������
	int clock_start;	//�ɻ���ɵ�ʱ��
	Plane_status state;
};
//�û������ã���һЩ�����������г�ʼ�������������ĵ�λʱ�䡢�������Ƴ��ȡ���λʱ�䵽�����ɵķɻ����ɵ�����
void initialize(int &end_time, int &queue_limit,double &arrival_rate, double &departure_rate)
void initialize(int &end_time, int &queue_limit, double &arrival_rate, double &departure_rate)	
{
	cout << "This program simulates an airport with only one runway." << endl
		<< "One plane can land or depart in each unit of time." << endl;

	cout << "Up to what number of planes can be waiting to land or take off at any time? " << flush;
	cin >> queue_limit;	//��ɺͽ���Ķ��еĳ��������ǣ�

	cout << "How many units of time will the simulation run?" << flush;
	cin >> end_time;	//�˴�ģ�������೤ʱ�䣿

	bool acceptable;

	do {	//�����Ϸ�ʱ����������������
		cout << "Expected number of arrivals per unit time?"<< flush;
		cin >> arrival_rate;	//ÿ����λʱ���ڵ�����ٷɻ���
		cout << "Expected number of departures per unit time?"<< flush;
		cin >> departure_rate;	//ÿ����λʱ���ڶ��ٷɻ�׼����ɣ�

		if (arrival_rate < 0.0 || departure_rate < 0.0)
			cerr << "These rates must be nonnegative." << endl;
		else
			acceptable = true;

		if (acceptable && arrival_rate + departure_rate > 1.0)
			cerr << "Safety Warning: This airport will become saturated. " << endl;
	} while (!acceptable);

}

// Runway Initialization
Runway::Runway(int limit)
// Post:  The Runway data members are initialized to record no prior
// Runway use and to record the limit on queue sizes.
//Runway class�Ĺ��캯����������ʼ���ܵ�

{
	queue_limit = limit;	//�û��Զ����ܵ������ɷɻ���
	num_land_requests = num_takeoff_requests = 0;	//�ȴ���ɺͽ���ķɻ��ĳ�ʼֵ
	num_landings = num_takeoffs = 0;	//�Ѿ��������ɵķɻ��ĳ�ʼֵ
	num_land_refused = num_takeoff_refused = 0;		//���ܾ���ɻ���ķɻ������ĳ�ʼֵ
	num_land_accepted = num_takeoff_accepted = 0;	//ͬ�⽵�����ɵķɻ������ĳ�ʼֵ
	land_wait = takeoff_wait = idle_time = 0;	//�ȴ��������ɵķɻ������ĳ�ʼֵ���ȴ�ʱ��
}

// Accepting a New Plane into the landing queue
Error_code Runway::can_land(const Plane &current)
// Post:  If possible, the Plane current is added to the landing Queue;
// otherwise, an Error_code of overflow is returned. The Runway statistics
// are updated.
// Uses:  class Queue.
//����һ�ܷɻ����˺����ķ���ֵ��Error_code������ȴ�����ķɻ�����С�ڶ��еĳ������ƣ��������ӣ���������Ѿ����ˣ���᷵��fail
{
	Error_code result;
	if (landing.size() < queue_limit)
		result = landing.append(current);	//����ǰ�ķɻ������ܵ��ȴ�����
	else
		result = fail;
	num_land_requests++;
	if (result != success)
		num_land_refused++;
	else
		num_land_accepted++;

	return result;
}

// Accepting a New Plane into the takeoff queue
Error_code Runway::can_depart(const Plane &current)
// Post:  If possible, the Plane current is added to the takeoff Queue;
// otherwise, an Error_code of overflow is returned. The Runway statistics
// are updated.
// Uses:  class Queue.

{
	Error_code result;
	if (takeoff.size() < queue_limit)
		result = takeoff.append(current);
	else
		result = fail;
	num_takeoff_requests++;
	if (result != success)
		num_takeoff_refused++;
	else
		num_takeoff_accepted++;

	return result;
}


Runway_activity Runway::activity(int time, Plane &moving)	//��ʾ�ɻ����ڸ�ɶ
// Post:  If the landing Queue has entries, its front Plane is copied to the
// parameter moving and a result land is returned. 
//Otherwise, if the takeoff Queue has entries, its front Plane is copied to the parameter
// moving and a result takeoff is returned. 
//Otherwise, idle is returned.Runway statistics are updated.
// Uses:  class Queue.
{
	Runway_activity in_progress;
	if (!landing.empty()) {		//���������в�Ϊ��
		landing.retrieve(moving);	//����ͷ�ķɻ����������������ɻ��ϣ�����ɻ�����
		land_wait += time - moving.started();
		num_landings++;
		in_progress = land;
		landing.serve();	//����
	}
	else if (!takeoff.empty()) {	//���������в�Ϊ�գ�����ɶ���Ϊ��
		takeoff.retrieve(moving);
		takeoff_wait += time - moving.started();
		num_takeoffs++;
		in_progress = takeoff;
		takeoff.serve();
	}
	else {		//������ɺͽ�����ж��ǿյģ�˵���ܵ�����������״̬
		idle_time++;		//����ʱ������һ����λ
		in_progress = idle;		//���ܵ���״̬����Ϊ����
	}
	return in_progress;
}


Plane::Plane(int flt, int time, Plane_status status)
// Post:  The Plane data members flt_num, clock_start, and state are set to
// the values of the parameters flt, time and status, respectively. 
//��ʼ��һ�ܷɻ�����Ҫ�ɻ���š�time�ͷɻ���״̬
{
	flt_num = flt;
	clock_start = time;
	state = status;
	cout << "Plane number " << flt << " ready to ";
	if (status == arriving)
		cout << "land." << endl;
	else
		cout << "take off." << endl;
}
Plane::Plane()
// Post:  The Plane data members flt_num, clock_start, state are set to 
// illegal default values.

{
	flt_num = -1;
	clock_start = -1;
	state = null;
}

// Refusing a plane
void Plane::refuse() const	//֪ͨ��ܷɻ����ܾ������
// Post: Processes a Plane wanting to use Runway, when the Queue is
// full.
{
	cout << "Plane number " << flt_num;
	if (state == arriving)		//�������׼������
		cout << " directed to another airport" << endl;		//������ȥ��Ļ���
	else		//���������Ҫ���
		cout << " told to try to takeoff again later" << endl;		//����������ٷ�
}

// Processing an arriving plane
void Plane::land(int time) const
// Post: Processes a Plane that is landing at the specified time.
{
	int wait = time - clock_start;		//������ҷɻ��ȴ��˶�òŽ��䣬�õ�ǰʱ���ȥ���������ʱ��
	cout << time << ": Plane number " << flt_num << " landed after " << wait << " time unit" << ((wait == 1) ? "" : "s") << " in the takeoff queue." << endl;
	//((wait == 1) ? "" : "s")���������﷨
}

// Processing a departing plane
void Plane::fly(int time) const
// Post: Process a Plane that is taking off at the specified time.
{
	int wait = time - clock_start;
	cout << time << ": Plane number " << flt_num << " took off after " << wait << " time unit" << ((wait == 1) ? "" : "s") << " in the takeoff queue." << endl;
}

// Communicating a plane;s arrival data
int Plane::started() const
// Post: Return the time that the Plane entered the airport system.
//���طɻ����������ʱ�䣬Ҳ���ǿ�ʼ��������ܵ���ʱ��
{
	return clock_start;
}

// Marking an idle time unit
void run_idle(int time)
// Post: The specified time is printed with a message that the runway is idle.
//֪ͨ�ܵ�Ŀǰ������
{
	cout << time << ": Runway is idle." << endl;
}

void Runway::shut_down(int time) const
// Post: Runway usage statistics are summarized and printed.
//֪ͨ�ܵ���ʹ�����
{
	cout << "Simulation has concluded after " << time << " time units." << endl
		<< "Total number of planes processed "
		<< (num_land_requests + num_takeoff_requests) << endl
		<< "Total number of planes asking to land "
		<< num_land_requests << endl
		<< "Total number of planes asking to take off "
		<< num_takeoff_requests << endl
		<< "Total number of planes accepted for landing "
		<< num_land_accepted << endl
		<< "Total number of planes accepted for takeoff "
		<< num_takeoff_accepted << endl
		<< "Total number of planes refused for landing "
		<< num_land_refused << endl
		<< "Total number of planes refused for takeoff "
		<< num_takeoff_refused << endl
		<< "Total number of planes that landed "
		<< num_landings << endl
		<< "Total number of planes that took off "
		<< num_takeoffs << endl
		<< "Total number of planes left in landing queue "
		<< landing.size() << endl
		<< "Total number of planes left in takeoff queue "
		<< takeoff.size() << endl;
	cout << "Percentage of time runway idle "
		<< 100.0 * ((float)idle_time) / ((float)time) << "%"
		<< endl;
	cout << "Average wait in landing queue "
		<< ((float)land_wait) / ((float)num_landings)
		<< " time units" << endl;
	cout << "Average wait in takeoff queue "
		<< ((float)takeoff_wait) / ((float)num_takeoffs)
		<< " time units" << endl;
	cout << "Average observed rate of planes wanting to land "
		<< ((float)num_land_requests) / ((float)time)
		<< " per time unit" << endl;
	cout << "Average observed rate of planes wanting to take off "
		<< ((float)num_takeoff_requests) / ((float)time)
		<< " per time unit" << endl;
}
int main() {
	int end_time;            //  time to run simulation ģ���ʱ��
	int queue_limit;         //  size of Runway queues �ܵ����еĳ���
	int flight_number = 0;
	double arrival_rate, departure_rate;	//�ɻ�������뿪������
	initialize(end_time, queue_limit, arrival_rate, departure_rate);	//�����ϵ�ֵ���г�ʼ��

	Random variable;
	Runway small_airport(queue_limit);	//����һ�������ܵ�
	for (int current_time = 0; current_time < end_time;current_time++)
	{
		//  loop over time intervals
		int number_arrivals = variable.poisson(arrival_rate);	//������λʱ��ɻ������ƽ��Ƶ�ʣ�������ɵ���ķɻ���������Ӳ���Ϊ��(lambda)�Ĳ��ɷֲ�
		//  current arrival requests
		for (int i = 0; i < number_arrivals; i++) {
			Plane current_plane(flight_number++, current_time, arriving);	//����һ�ܵ�ǰ�ɻ���״̬�ǵȴ�����
			if (small_airport.can_land(current_plane) != success)	//����ɻ������ܵ��ȴ����в��ɹ�
				current_plane.refuse();		//֪ͨ��ܷɻ����ܾ���
		}
		int number_departures = variable.poisson(departure_rate);		//������λʱ��ɻ���ɵ�ƽ��Ƶ�ʣ�������ɵȴ���ɵķɻ���������Ӳ���Ϊ��(lambda)�Ĳ��ɷֲ�
		//current departure requests
		for (int j = 0; j < number_departures; j++) {
			Plane current_plane(flight_number++, current_time, departing);	//����һ�ܵ�ǰ�ɻ���״̬�ǵȴ����
			if (small_airport.can_depart(current_plane) != success)		//����ɻ������ܵ��ȴ����в��ɹ�
				current_plane.refuse();		//֪ͨ��ܷɻ����ܾ���
		}

		Plane moving_plane;
		switch (small_airport.activity(current_time, moving_plane))
		{
			//  Let at most one Plane onto the Runway at current_time.
		case land:		//�зɻ��������
			moving_plane.land(current_time);		//֪ͨ
			break;
		case takeoff:		//�зɻ����ڽ���
			moving_plane.fly(current_time);
			break;
		case idle:		//�����ܵ�������
			run_idle(current_time);		//�������õ�״̬
		}
	} //  end of loop over time intervals
	//ÿһ����λʱ����ֻ����ɻ���һ�ܷɻ�
	small_airport.shut_down(end_time);	//�رջ����ܵ���������
}
