#include "tpe/task_pool_executor.h"

#include <unistd.h>
//#include <pthread.h>
//#include <mach/thread_policy.h>
//#include <mach/thread_act.h>
//#include <sched.h>



namespace task {
namespace pool {

TaskPoolExecutor::TaskPoolExecutor():
    MainPool_t(),
    _is_initialized(false)
{
}

void TaskPoolExecutor::init() {
  if (_is_initialized) {
      return;
  } else {
      _is_initialized = true;
  }
  int cpu_count = std::thread::hardware_concurrency();
  size_t pool_size = std::max(cpu_count - 1, 1);
  MainPool_t::init(
      pool_size,
      [&](std::shared_ptr<MainTaskBase> tsk) {
	tsk->execute(*_sub_pools[tsk->current_queue_idx()]);
  });

  for (int i = 0 ; i < size() ; ++i) {
      auto sub_pool = std::unique_ptr<SubPool_t>(new SubPool_t());
      sub_pool->init(pool_size, [](
	  std::shared_ptr<SubTaskBase> tsk) {
	tsk->execute();
      });
      _sub_pools.push_back(std::move(sub_pool));
  }

//  for (int cpu = 1 ;
//      cpu < cpu_count ;
//      ++cpu) {
//      _thread_pool.push_back(
//	  std::unique_ptr<utils::concurrency::QueueThread<
//	      task::TaskBase>>(
//	      new utils::concurrency::QueueThread<task::TaskBase>(
//	      [](std::shared_ptr<task::TaskBase> tsk){
//	tsk->execute();
//      })));

//      thread_affinity_policy_data_t policy_data = { i };
//      std::cout << "setting thread "<<
//	  pool_thread->get_id() <<
//	  " to cpu "<< policy_data.affinity_tag << std::endl;
//      thread_policy_set(*(thread_act_t*)pool_thread->native_handle(),
//    		    THREAD_AFFINITY_POLICY,
//    		    (thread_policy_t)&policy_data,
//    		    THREAD_AFFINITY_POLICY_COUNT);
}

void TaskPoolExecutor::enqueue_task(
    std::shared_ptr<task::MainTaskBase> task
)
{
  int queue_idx = get_available_queue();
  task->before_execution(queue_idx);
  enqueue_item(task, queue_idx);
}

//void TaskPoolExecutor::print_thread_status(void) {
//  std::for_each(
//      _thread_pool.begin(),
//      _thread_pool.end(),
//      [](std::thread *pool_thread) {
//	thread_affinity_policy_data_t policy_data;
//	boolean_t t_type;
//	mach_msg_type_number_t msg_type_num;
//	thread_policy_get(*(thread_inspect_t*)pool_thread->native_handle(),
//			  THREAD_AFFINITY_POLICY,
//			  (thread_policy_t)&policy_data,
//			  &msg_type_num,
//			  &t_type);
//	std::cout << "thread " <<
//	    pool_thread->get_id() <<
//	    " is running on cpu " << policy_data.affinity_tag <<
//	    std::endl;
//      });
//

}
}
