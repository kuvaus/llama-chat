
add_executable(chat chat.cpp header.h utils.h parse_json.h ../llm/llamamodel.h ../llm/llamamodel.cpp ../llama.cpp/examples/common.cpp ../llm/llmodel.h  )
target_link_libraries(chat PRIVATE  llama)


