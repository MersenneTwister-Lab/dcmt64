#pragma once
#ifndef SEARCH_H
#define SEARCH_H

#include <iostream>

#include "options.h"

int search(options& opt, std::ostream& os, std::ostream& log, int count);
int best_search(options& opt, std::ostream& os, std::ostream& log, int count);

#endif // SEARCH_H
