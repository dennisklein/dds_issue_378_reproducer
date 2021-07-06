#include <chrono>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <dds/dds.h>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main()
{
  dds::intercom_api::CIntercomService service;
  dds::intercom_api::CCustomCmd cc(service);
  std::string const sid(dds::env_prop<dds::dds_session_id>());

  std::cerr << "$DDS_TASK_PATH: " << dds::env_prop<dds::task_path>() << '\n';
  std::cerr << "$DDS_GROUP_NAME: " << dds::env_prop<dds::group_name>() << '\n';
  std::cerr << "$DDS_COLLECTION_NAME: " << dds::env_prop<dds::collection_name>() << '\n';
  std::cerr << "$DDS_TASK_NAME: " << dds::env_prop<dds::task_name>() << '\n';
  std::cerr << "$DDS_TASK_INDEX: " << dds::env_prop<dds::task_index>() << '\n';
  std::cerr << "$DDS_COLLECTION_INDEX: " << dds::env_prop<dds::collection_index>() << '\n';
  std::cerr << "$DDS_TASK_ID: " << dds::env_prop<dds::task_id>() << '\n';
  std::cerr << "$DDS_LOCATION: " << dds::env_prop<dds::dds_location>() << '\n';
  std::cerr << "$DDS_SESSION_ID: " << sid << '\n';

  service.subscribeOnError([](dds::intercom_api::EErrorCode const ec,
                              std::string const & msg) {
    std::cerr << "DDS error (" << ec << "): " << msg << '\n';
  });

  cc.subscribe([&sid, &cc](std::string const& msg,
                           std::string const& condition,
                           uint64_t sender_id) {
    std::cerr << ">>> CC(" << sid << "): " << msg << ", "
              << "condition: " << condition << ", "
              << "sender_id: " << sender_id << '\n';
    cc.send(sid, std::to_string(sender_id));
  });

  service.start(sid);

  std::this_thread::sleep_for(60s);

  std::cerr << "exiting after sleep\n";

  cc.unsubscribe();

  return 0;
}
