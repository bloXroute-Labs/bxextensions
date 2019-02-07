#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/decryption_task.h>

#ifndef SRC_TASK_DECRYPTION_TASK_H_
#define SRC_TASK_DECRYPTION_TASK_H_

typedef task::DecryptionTask DecryptionTask_t;
typedef std::shared_ptr<DecryptionTask_t> PDecryptionTask_t;

void bind_decryption_task(py::module& m);

#endif /* SRC_TASK_DECRYPTION_TASK_H_ */
