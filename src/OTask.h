<<<<<<< HEAD
#ifndef _TASK_H_
#define _TASK_H_
// ��������࣬�̳и�����������ʵ�ִ��麯��ORun()
class OTask
{
public:
	OTask(int id) { _TaskId = id; }

	~OTask() { }

public:
	virtual void ORun(int i) = 0;

	int OTaskId() { return _TaskId; }
private:
	int _TaskId;
};
#endif  /*_TASK_H_*/
=======
#ifndef _TASK_H_
#define _TASK_H_
// ��������࣬�̳и�����������ʵ�ִ��麯��ORun()
class OTask
{
public:
	OTask(int id) { _TaskId = id; }

	~OTask() { }

public:
	virtual void ORun(int i) = 0;

	int OTaskId() { return _TaskId; }
private:
	int _TaskId;
};
#endif  /*_TASK_H_*/
>>>>>>> b42339f86429495f7f8c0eabe515682d6c9388ad
