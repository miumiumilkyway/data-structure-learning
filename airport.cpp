#include<bits/stdc++.h>
using namespace std;


enum Runway_activity { idle, land, takeoff };	//Enumerates all Runway's states
class Runway {
public:
	Runway(int limit);	//constructor:initialize the Runway
	Error_code can_land(const Plane &current);	//判断landing queue是否还能加入飞机
	Error_code can_depart(const Plane &current);	//判断takeoff queue是否还能加入飞机
	Runway_activity activity(int time, Plane &moving);	//判断当前跑道正在干什么
	void shut_down(int time) const;		//所有单位时间结束后通报总结一次

private:
	Queue landing;    //  landing queue
	Queue takeoff;     //  takeoff queue
	int queue_limit;	// 队列长度限制，起飞队列和降落队列是一样的

	//以下变量基本用于最后的通报，shut down函数
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
	void refuse() const;	//表示某架飞机在某个单位时间内的状态：被拒绝起飞/降落，继续等待
	void land(int time) const;		//表示某架飞机在某个单位时间内的状态：降落，函数的作用主要是通报，真正的出队操作不在这里完成
	void fly(int time) const;		//表示某架飞机在某个单位时间内的状态：起飞，函数的作用主要是通报，真正的出队操作不在这里完成
	int started() const;	//返回飞机起飞的时间，也就是clock_start

private:
	int flt_num;	//航班编号
	int clock_start;	//飞机起飞的时间
	Plane_status state;
};
//用户来设置，对一些限制条件进行初始化，包括结束的单位时间、队列限制长度、单位时间到达和起飞的飞机生成的速率
void initialize(int &end_time, int &queue_limit,double &arrival_rate, double &departure_rate)
void initialize(int &end_time, int &queue_limit, double &arrival_rate, double &departure_rate)	
{
	cout << "This program simulates an airport with only one runway." << endl
		<< "One plane can land or depart in each unit of time." << endl;

	cout << "Up to what number of planes can be waiting to land or take off at any time? " << flush;
	cin >> queue_limit;	//起飞和降落的队列的长度限制是？

	cout << "How many units of time will the simulation run?" << flush;
	cin >> end_time;	//此次模拟会持续多长时间？

	bool acceptable;

	do {	//当不合法时，会重新让你输入
		cout << "Expected number of arrivals per unit time?"<< flush;
		cin >> arrival_rate;	//每个单位时间内到达多少飞机？
		cout << "Expected number of departures per unit time?"<< flush;
		cin >> departure_rate;	//每个单位时间内多少飞机准备起飞？

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
//Runway class的构造函数，用来初始化跑道

{
	queue_limit = limit;	//用户自定义跑道可容纳飞机数
	num_land_requests = num_takeoff_requests = 0;	//等待起飞和降落的飞机的初始值
	num_landings = num_takeoffs = 0;	//已经降落或起飞的飞机的初始值
	num_land_refused = num_takeoff_refused = 0;		//被拒绝起飞或降落的飞机数量的初始值
	num_land_accepted = num_takeoff_accepted = 0;	//同意降落或起飞的飞机数量的初始值
	land_wait = takeoff_wait = idle_time = 0;	//等待降落或起飞的飞机数量的初始值、等待时间
}

// Accepting a New Plane into the landing queue
Error_code Runway::can_land(const Plane &current)
// Post:  If possible, the Plane current is added to the landing Queue;
// otherwise, an Error_code of overflow is returned. The Runway statistics
// are updated.
// Uses:  class Queue.
//传入一架飞机，此函数的返回值是Error_code，如果等待降落的飞机数量小于队列的长度限制，则可以入队，如果现在已经满了，则会返回fail
{
	Error_code result;
	if (landing.size() < queue_limit)
		result = landing.append(current);	//将当前的飞机加入跑道等待队列
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


Runway_activity Runway::activity(int time, Plane &moving)	//表示飞机正在干啥
// Post:  If the landing Queue has entries, its front Plane is copied to the
// parameter moving and a result land is returned. 
//Otherwise, if the takeoff Queue has entries, its front Plane is copied to the parameter
// moving and a result takeoff is returned. 
//Otherwise, idle is returned.Runway statistics are updated.
// Uses:  class Queue.
{
	Runway_activity in_progress;
	if (!landing.empty()) {		//如果降落队列不为空
		landing.retrieve(moving);	//队列头的飞机被拷贝到这架虚拟飞机上，虚拟飞机降落
		land_wait += time - moving.started();
		num_landings++;
		in_progress = land;
		landing.serve();	//出队
	}
	else if (!takeoff.empty()) {	//如果降落队列不为空，但起飞队列为空
		takeoff.retrieve(moving);
		takeoff_wait += time - moving.started();
		num_takeoffs++;
		in_progress = takeoff;
		takeoff.serve();
	}
	else {		//假设起飞和降落队列都是空的，说明跑道正处于闲置状态
		idle_time++;		//闲置时长增加一个单位
		in_progress = idle;		//将跑道的状态设置为闲置
	}
	return in_progress;
}


Plane::Plane(int flt, int time, Plane_status status)
// Post:  The Plane data members flt_num, clock_start, and state are set to
// the values of the parameters flt, time and status, respectively. 
//初始化一架飞机，需要飞机编号、time和飞机的状态
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
void Plane::refuse() const	//通知这架飞机被拒绝入队了
// Post: Processes a Plane wanting to use Runway, when the Queue is
// full.
{
	cout << "Plane number " << flt_num;
	if (state == arriving)		//如果你是准备降落
		cout << " directed to another airport" << endl;		//那请你去别的机场
	else		//如果你是想要起飞
		cout << " told to try to takeoff again later" << endl;		//那请你等下再飞
}

// Processing an arriving plane
void Plane::land(int time) const
// Post: Processes a Plane that is landing at the specified time.
{
	int wait = time - clock_start;		//计算这家飞机等待了多久才降落，用当前时间减去发出请求的时间
	cout << time << ": Plane number " << flt_num << " landed after " << wait << " time unit" << ((wait == 1) ? "" : "s") << " in the takeoff queue." << endl;
	//((wait == 1) ? "" : "s")用来调整语法
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
//返回飞机到达机场的时间，也就是开始请求进入跑道的时间
{
	return clock_start;
}

// Marking an idle time unit
void run_idle(int time)
// Post: The specified time is printed with a message that the runway is idle.
//通知跑道目前闲置了
{
	cout << time << ": Runway is idle." << endl;
}

void Runway::shut_down(int time) const
// Post: Runway usage statistics are summarized and printed.
//通知跑道的使用情况
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
	int end_time;            //  time to run simulation 模拟的时长
	int queue_limit;         //  size of Runway queues 跑道队列的长度
	int flight_number = 0;
	double arrival_rate, departure_rate;	//飞机到达和离开的速率
	initialize(end_time, queue_limit, arrival_rate, departure_rate);	//对以上的值进行初始化

	Random variable;
	Runway small_airport(queue_limit);	//创建一条机场跑道
	for (int current_time = 0; current_time < end_time;current_time++)
	{
		//  loop over time intervals
		int number_arrivals = variable.poisson(arrival_rate);	//给定单位时间飞机到达的平均频率，随机生成到达的飞机数，其服从参数为λ(lambda)的泊松分布
		//  current arrival requests
		for (int i = 0; i < number_arrivals; i++) {
			Plane current_plane(flight_number++, current_time, arriving);	//生成一架当前飞机，状态是等待降落
			if (small_airport.can_land(current_plane) != success)	//如果飞机加入跑道等待队列不成功
				current_plane.refuse();		//通知这架飞机被拒绝了
		}
		int number_departures = variable.poisson(departure_rate);		//给定单位时间飞机起飞的平均频率，随机生成等待起飞的飞机数，其服从参数为λ(lambda)的泊松分布
		//current departure requests
		for (int j = 0; j < number_departures; j++) {
			Plane current_plane(flight_number++, current_time, departing);	//生成一架当前飞机，状态是等待起飞
			if (small_airport.can_depart(current_plane) != success)		//如果飞机加入跑道等待队列不成功
				current_plane.refuse();		//通知这架飞机被拒绝了
		}

		Plane moving_plane;
		switch (small_airport.activity(current_time, moving_plane))
		{
			//  Let at most one Plane onto the Runway at current_time.
		case land:		//有飞机正在起飞
			moving_plane.land(current_time);		//通知
			break;
		case takeoff:		//有飞机正在降落
			moving_plane.fly(current_time);
			break;
		case idle:		//机场跑道闲置中
			run_idle(current_time);		//播报闲置的状态
		}
	} //  end of loop over time intervals
	//每一个单位时间内只能起飞或降落一架飞机
	small_airport.shut_down(end_time);	//关闭机场跑道咯，嘻嘻
}
