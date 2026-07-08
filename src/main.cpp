#include "utilities.h"
#include "mmpbsa.h"


int main(int argc, char** argv)
{
    RepeatCommandLineCall(argc, argv);

    Settings settings(argc, argv);
    settings.Validate();

    if (settings.PREPARE_JOB)
    {
        // prepare_mmpbsa_inputs
        prepare_mmpbsa_job(settings);
    }

    if (settings.RUN_JOB)
    {
        run_mmpbsa_job(settings);
    }

    return 0;
}