#include <array>
#include <atomic>
#include <chrono>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <dds/dds.h>
#include <iostream>
#include <memory>
#include "semaphore.hpp"
#include <string>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;

namespace di378 {

struct session
{
  struct slots_count {
    std::uint32_t idle = 0;
    std::uint32_t active = 0;
    std::uint32_t executing = 0;
  };

  auto get_slots_count()
  {
    using namespace dds::tools_api;

    SAgentCountRequest::response_t res;
    m_session->syncSendRequest<SAgentCountRequest>(SAgentCountRequest::request_t(), res);

    return slots_count{res.m_activeSlotsCount,
                       res.m_idleSlotsCount,
                       res.m_executingSlotsCount};
  }

  auto wait_for_idle_slots()
  {
    auto slots(get_slots_count());
    while (slots.idle < 1) {
        std::this_thread::sleep_for(10ms);
        slots = get_slots_count();
    }
  }

  auto submit_agents(std::shared_ptr<semaphore> blocker)
  {
    using namespace dds::tools_api;

    std::uint32_t agents(1);
    std::uint32_t slots(1);
    SSubmitRequestData info;
    info.m_rms = "localhost";
    info.m_instances = agents;
    info.m_slots = slots;
    info.m_config = "";

    auto const request(SSubmitRequest::makeRequest(info));
    // request->setMessageCallback([](SMessageResponseData const & message){
      // std::cerr << message.m_msg << '\n';
    // });
    request->setDoneCallback([agents, slots, blocker]() mutable {
        // std::cerr << agents << " agents with " << slots << " slots each submitted\n";
        blocker->signal();
    });

    m_session->sendRequest<SSubmitRequest>(request);
  }

  auto activate_topology(std::shared_ptr<semaphore> blocker)
  {
    using namespace dds::tools_api;

    STopologyRequestData info;
    info.m_updateType = STopologyRequestData::EUpdateType::ACTIVATE;
    info.m_topologyFile = "/home/dklein/projects/dds_issue_378_reproducer/reproducer.xml";

    auto request(STopologyRequest::makeRequest(info));
    // request->setMessageCallback([](SMessageResponseData const & _message) {
      // std::cerr << _message.m_msg << '\n';
    // });
    request->setDoneCallback([blocker]() mutable { blocker->signal(); });
    m_session->sendRequest<STopologyRequest>(request);
  }

  auto cc_send(std::string const & msg)
  {
    m_cc.send(msg, "");
    // std::cerr << ">>> CC(" << m_id << "): " << msg << '\n';
  }

  session()
    : m_session(std::make_unique<dds::tools_api::CSession>())
    , m_cc(m_service)
    , m_id(to_string(m_session->create()))
    , m_msg_count(0)
  {
    m_service.subscribeOnError([](dds::intercom_api::EErrorCode const ec,
                                  std::string const & msg) {
      std::cerr << "DDS error (" << ec << "): " << msg << '\n';
    });

    m_cc.subscribe([this](std::string const& msg,
                          std::string const& condition,
                          uint64_t sender_id) {
      ++m_msg_count;
      if (msg != m_id) {
        std::stringstream ss;
        ss << "<<< CC(" << m_id << "): " << msg << ", "
           << "condition: " << condition << ", "
           << "sender_id: " << sender_id << '\n';
        std::cerr << ss.str();
      } else {
        // std::cerr << '.';
      }
    });

    m_service.start(m_id);

    std::cerr << m_id << '\n';
  }

  ~session()
  {
    m_session->shutdown();
    std::cerr << m_id << " received msgs: " << m_msg_count << '\n';
  }

  std::unique_ptr<dds::tools_api::CSession> m_session;
  dds::intercom_api::CIntercomService m_service;
  dds::intercom_api::CCustomCmd m_cc;
  std::string m_id;
  std::atomic<std::size_t> m_msg_count;
};

}

int main()
{
  constexpr auto num_sessions(6);
  constexpr auto num_msgs_per_session(10000);
  std::array<di378::session, num_sessions> sessions;
  auto blocker(std::make_shared<di378::semaphore>());

  for(auto& session : sessions) {
    session.submit_agents(blocker);
  }
  blocker->wait(num_sessions);

  for(auto& session : sessions) {
    session.wait_for_idle_slots();
  }

  for(auto& session : sessions) {
    session.activate_topology(blocker);
  }
  blocker->wait(num_sessions);

  for(auto i(0); i < num_msgs_per_session; ++i) {
    for(auto& session : sessions) {
      session.cc_send(session.m_id);
    }
  }

  std::this_thread::sleep_for(5s);
  std::cerr << "exiting ...\n";

  return 0;
}
