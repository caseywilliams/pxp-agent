#ifndef SRC_UTIL_BOLT_MODULE_H_
#define SRC_UTIL_BOLT_MODULE_H_

#include <pxp-agent/action_response.hpp>
#include <pxp-agent/module.hpp>
#include <pxp-agent/results_storage.hpp>

#include <leatherman/execution/execution.hpp>

#include <boost/filesystem.hpp>

namespace PXPAgent {
namespace Util {

struct CommandObject {
    std::string executable;
    std::vector<std::string> arguments;
    std::map<std::string, std::string> environment;
    std::string input;
    std::function<void(size_t)> pid_callback;
};

class BoltModule : public PXPAgent::Module {
    public:
        explicit BoltModule(std::shared_ptr<ResultsStorage> storage)
           : storage_(std::move(storage)) {}

        /// Whether the module supports non-blocking / asynchronous requests.
        bool supportsAsync() override { return true; }

        /// Log information about the output of the performed action
        /// while validating the output is valid UTF-8.
        /// Update the metadata of the ActionResponse instance (the
        /// 'results_are_valid', 'status', and 'execution_error' entries
        /// will be set appropriately; 'end' will be set to the current
        /// time).
        /// This function does not throw a ProcessingError in case of
        /// invalid output on stdout; such failure is instead reported
        /// in the response object's metadata.
        void processOutputAndUpdateMetadata(ActionResponse& response) override;

    protected:
        std::shared_ptr<ResultsStorage> storage_;

        // Construct a CommandObject based on a request
        virtual CommandObject buildCommandObject(const ActionRequest& request) = 0;

        // Execute a CommandObject synchronously
        virtual leatherman::execution::result run(const CommandObject& cmd);

        // Execute a CommandObject, spawning a new process
        virtual leatherman::execution::result run_async(const CommandObject& cmd);

        virtual void callBlockingAction(
                const ActionRequest& request,
                const Util::CommandObject &command,
                ActionResponse &response);

        virtual void callNonBlockingAction(
                const ActionRequest& request,
                const CommandObject &command,
                ActionResponse &response);

        ActionResponse callAction(const ActionRequest& request) override;
};

}  // namespace Util
}  // namespace PXPAgent

#endif
