/*
 * Copyright (C) 2015, Simon Fuhrmann
 * TU Darmstadt - Graphics, Capture and Massively Parallel Computing
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-Clause license. See the LICENSE.txt file for details.
 */

#include <chrono>

#include "fancy_progress_printer.h"
#include "util/system.h"

#define ANSI_CURSOR_RESET "\x1B[H"
#define ANSI_CLEAR_SCREEN "\x1B[2J"

#define ANSI_STYLE_RESET "\x1B[0m"
#define ANSI_STYLE_BOLD "\x1B[1m"

#define ANSI_STYLE_BLACK "\x1B[30m"
#define ANSI_STYLE_RED "\x1B[31m"
#define ANSI_STYLE_GREEN "\x1B[32m"
#define ANSI_STYLE_YELLOW "\x1B[33m"
#define ANSI_STYLE_BLUE "\x1B[34m"
#define ANSI_STYLE_MAGENTA "\x1B[35m"
#define ANSI_STYLE_CYAN "\x1B[36m"
#define ANSI_STYLE_WHITE "\x1B[37m"

void
FancyProgressPrinter::start()
{
    thread = std::thread(&FancyProgressPrinter::run, this);
    thread.detach();
}

void
FancyProgressPrinter::run()
{
    this->lastCompleted = -1;
    this->isRunning = true;
    while (this->isRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        this->print();
    }
}

void
FancyProgressPrinter::print()
{
    std::lock_guard<std::mutex> lock(this->mutex);

    int completed = 0;
    int queued = 0;

    for (std::vector<ViewStatus>::iterator it = this->viewStatus.begin();
        it != this->viewStatus.end(); ++it)
    {
        switch (*it)
        {
        case STATUS_IGNORED:
            completed += 1;
            break;
        case STATUS_QUEUED:
            queued += 1;
            break;
        case STATUS_IN_PROGRESS:
            queued += 1;
            break;
        case STATUS_DONE:
            completed += 1;
            break;
        case STATUS_FAILED:
            completed += 1;
            break;
        }
    }

    if (this->lastCompleted != completed){
        std::cout << completed << " of " << (completed + queued) << " completed (" << std::fixed << std::setprecision(2) << (100.0f * static_cast<float>(completed) / static_cast<float>(completed + queued)) << "%)" << std::endl;
        this->lastCompleted = completed;
    }

    // for (std::set<mvs::DMRecon const*>::iterator it = this->runningRecons.begin();
    //     it != this->runningRecons.end(); ++it)
    // {
    //     mvs::Progress const &p = (*it)->getProgress();
    //     std::cout << "View #" << (*it)->getRefViewNr()
    //               << ": filled " << p.filled
    //               << " of " << (p.filled + p.queueSize) << '\n';
    // }
}
