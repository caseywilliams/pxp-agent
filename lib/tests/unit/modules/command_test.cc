#include "root_path.hpp"
#include "../../common/content_format.hpp"

#include <pxp-agent/modules/command.hpp>
#include <leatherman/execution/execution.hpp>

#include <catch.hpp>

using namespace PXPAgent;

namespace lth_exe = leatherman::execution;
namespace lth_jc = leatherman::json_container;

static const std::string SPOOL_DIR { std::string { PXP_AGENT_ROOT_PATH }
                                     + "/lib/tests/resources/test_spool" };
static const auto STORAGE = std::make_shared<ResultsStorage>(SPOOL_DIR, "0d");

namespace PXPAgent { namespace Modules {
    class MockedCommand : public Command {
    public:
        // Mock the execution result that run() will return
        leatherman::execution::result mock_result_;

        MockedCommand(bool success, const std::string &output, const std::string &error, int exit_code, size_t pid) :
            Command(STORAGE),
            mock_result_(leatherman::execution::result { success, output, error, exit_code, pid }) {
        }

    private:
        lth_exe::result run(const Util::CommandObject& cmd) override {
            return mock_result_;
        }

        lth_exe::result run_async(const Util::CommandObject& cmd) override {
            return mock_result_;
        }
    };
}}

TEST_CASE("Modules::Command", "[modules]") {
    SECTION("can successfully instantiate") {
        REQUIRE_NOTHROW(Modules::Command(STORAGE));
    }
}

TEST_CASE("Modules::Command::hasAction", "[modules]") {
    Modules::Command mod {STORAGE};

    SECTION("correctly reports false for a nonexistent action") {
        REQUIRE_FALSE(mod.hasAction("foo"));
    }

    SECTION("correctly reports true for a real action") {
        REQUIRE(mod.hasAction("run"));
    }
}

TEST_CASE("Modules::Command::callAction", "[modules]") {
    Modules::MockedCommand mod { true, "root", "", 0, 9999 };

    SECTION("executes a command") {
        auto command_txt = (DATA_FORMAT % "\"0987\""
                % "\"command\""
                % "\"run\""
                % "{\"command\": \"whoami\"}").str();
        PCPClient::ParsedChunks command_content {
            lth_jc::JsonContainer(ENVELOPE_TXT),
            lth_jc::JsonContainer(command_txt),
            {},
            0
        };

        ActionRequest request { RequestType::Blocking, command_content };
        auto response = mod.executeAction(request);
        auto output = response.action_metadata.toString();
        auto stdout = response.action_metadata.get<std::string>({"results", "stdout"});

        REQUIRE(stdout == "root");
    };
}
