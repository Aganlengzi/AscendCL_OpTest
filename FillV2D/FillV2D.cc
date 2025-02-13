#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include <vector>
#include <numeric>

#include "common/nputensor.h"

int main() {
  // Init
  ACL_CALL(aclInit(nullptr));
  ACL_CALL(aclrtSetDevice(0));

  // op type
  const std::string op_type = "FillV2D";
  // output
  const std::vector<int64_t> output_dims{1,};

  // input - value

  // set inputs desc and buffer
  std::vector<aclTensorDesc *> input_descs;
  std::vector<aclDataBuffer *> input_buffers;

  // output - out
  auto output = new npuTensor<int64_t>(ACL_INT64, output_dims.size(), output_dims.data(), ACL_FORMAT_ND, nullptr);

  // set output desc and buffer
  std::vector<aclTensorDesc *> output_descs;
  std::vector<aclDataBuffer *> output_buffers;
  output_descs.emplace_back(output->desc);
  output_buffers.emplace_back(output->buffer);

  // attr
  auto attr = aclopCreateAttr();
  ACL_CALL(aclopSetAttrFloat(attr, "value", 0.5f));
  ACL_CALL(aclopSetAttrListInt(attr, "dims", output_dims.size(), output_dims.data()));
  
  // create stream
  aclrtStream stream = nullptr;
  ACL_CALL(aclrtCreateStream(&stream));

  std::cout << "aclopCompileAndExecute : " << op_type << std::endl;
  ACL_CALL(aclopCompileAndExecute(op_type.c_str(), 
            input_descs.size(), input_descs.data(), input_buffers.data(), 
            output_descs.size(), output_descs.data(), output_buffers.data(), 
            attr, ACL_ENGINE_SYS, ACL_COMPILE_SYS, NULL, stream));

  // sync and destroy stream
  ACL_CALL(aclrtSynchronizeStream(stream));
  ACL_CALL(aclrtDestroyStream(stream));

  // print output
  output->Print("y");

  // destroy - outputs
  output->Destroy();

  aclopDestroyAttr(attr);

  // release
  ACL_CALL(aclrtResetDevice(0));
  ACL_CALL(aclFinalize());

  return 0;
}
