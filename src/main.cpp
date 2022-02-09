// Build options (Coming soon)
// SFV_READ_ONLY - Read only program
// SFV_WRITE_ONLY - Write only program
// SFV_READ_WRITE - Read Write

#define SFV_READ_WRITE

#include <iostream>
#include <sfv/SFVReader.h>
#include <sfv/SFVWriter.h>
#include <utils/SimpleArguments.h>
#include <utils/Timer.h>

void print_help() {
    std::cout << "sfv archiver" << "\n";
#if defined(SFV_READ_WRITE) || defined(SFV_READ_ONLY)
    std::cout << "--readSFV read SFV file" << "\n";
#endif
#if defined(SFV_READ_WRITE) || defined(SFV_WRITE_ONLY)
    std::cout << "--writeSFV create a SFV file" << "\n";
#endif
}

int main(int argc, char* argv[]) {
    if(argc < 2){
        print_help();
        return 0;
    }

    SimpleArguments simple_args(argc, argv);

    bool log_only_final_results = simple_args.find("-r");

    unsigned int thread_count = 0;

    if (simple_args.find("-t")) {
        thread_count = std::stoi(simple_args.findAfter("-t"));
    }

    if (simple_args.count() == 1) {
	    if (std::filesystem::path possible_sfv_file = simple_args.getFirst(); std::filesystem::is_regular_file(simple_args.getFirst()) && (possible_sfv_file.extension() == ".sfv")) {
            Timer timer;
            timer.start();
            SFVReader sfv_reader(simple_args.findAfter("--readSFV"), log_only_final_results);
            sfv_reader.setThreadCount(thread_count);
            sfv_reader.process();
            timer.stopAndPrint();
            return 0;
        }
    }

    if (simple_args.find("--readSFV")) {
        Timer timer;
        timer.start();
        SFVReader sfv_reader(simple_args.findAfter("--readSFV"), log_only_final_results);
        sfv_reader.setThreadCount(thread_count);
        sfv_reader.process();
        timer.stopAndPrint();
        return 0;
    }

    if (simple_args.find("--writeSFV")) {
        Timer timer;
        timer.start();
        SFVWriter sfv_writer(simple_args.findAfter("--writeSFV"), log_only_final_results);
        sfv_writer.setThreadCount(thread_count);
        sfv_writer.process();
        timer.stopAndPrint();
        return 0;
    }

    print_help();
    return 0;
}
