#include <pxp-agent/modules/command.hpp>
#include <pxp-agent/module_type.hpp>
#include <pxp-agent/util/utf8.hpp>

#include <leatherman/locale/locale.hpp>
#include <leatherman/execution/execution.hpp>

#define LEATHERMAN_LOGGING_NAMESPACE "puppetlabs.pxp_agent.modules.command"
#include <leatherman/logging/logging.hpp>

#include <utility>  // for std::move

namespace PXPAgent {
namespace Modules {

namespace lth_exec = leatherman::execution;
namespace lth_jc = leatherman::json_container;
namespace lth_loc  = leatherman::locale;

static const std::string COMMAND_RUN_ACTION { "run" };

Command::Command(std::shared_ptr<ResultsStorage> storage) : BoltModule { std::move(storage) }
{
    module_name = "command";
    actions.push_back(COMMAND_RUN_ACTION);

    PCPClient::Schema input_schema { COMMAND_RUN_ACTION };
    input_schema.addConstraint("command", PCPClient::TypeConstraint::String, true);
    input_validator_.registerSchema(input_schema);

    PCPClient::Schema output_schema { COMMAND_RUN_ACTION };
    results_validator_.registerSchema(output_schema);
}

Util::CommandObject Command::buildCommandObject(const ActionRequest& request)
{
    auto params = request.params();

    assert(params.includes("command") &&
           params.type("command") == lth_jc::DataType::String);

    return {
        params.get<std::string>("command"),
        {},
        {},
        "",
        nullptr
    };
}

}  // namespace Modules
}  // namespace PXPAgent
