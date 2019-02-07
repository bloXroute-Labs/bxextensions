#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <iostream>

#include <memory>

#include <tpe/task/encryption_task.h>

#ifndef SRC_TASK_ENCRYPTION_TASK_H_
#define SRC_TASK_ENCRYPTION_TASK_H_

typedef task::EncryptionTask EncryptionTask_t;
typedef std::shared_ptr<EncryptionTask_t> PEncryptionTask_t;

void bind_encryption_task(py::module& m);

#endif /* SRC_TASK_ENCRYPTION_TASK_H_ */
