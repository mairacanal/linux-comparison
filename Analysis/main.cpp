#include <array>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include <matplot/matplot.h>

using namespace std;

typedef struct line {
  double time = 0;
  bool input = false;
  bool output = false;
  bool is_valid = false;
} line;

typedef struct interval {
  double start = 0;
  double end = 0;
  double time = 0;
  bool signal = false;
  int id = 0;
} interval;

typedef struct report {
  interval req;
  interval res;
  double delay = 0;
  double jitter = 0;
} report;

void print_line(const line &line) {
  cout << line.time << ',' << line.input << ',' << line.output;
}

void print_interval(const interval &interval) {
  cout << interval.id << ",";
  cout << interval.start << ',';
  cout << interval.end << ',';
  cout << interval.time;
}

void print_report(const report &report) {
  print_interval(report.req);
  cout << ',';
  print_interval(report.res);
  cout << ',';
  cout << report.delay << ',';
  cout << report.jitter << '\n';
}

auto read_header(int number_of_fields) -> vector<string> {
  vector<string> header;
  string header_field;

  for (int i = 0; i < number_of_fields - 1; i++) {
    std::getline(cin, header_field, ',');
    header.push_back(header_field);
  }

  std::getline(cin, header_field);
  header.push_back(header_field);

  return header;
}

auto read_line(line &curr_line) {
  char comma;

  if (!(cin >> curr_line.time >> comma)) {
    return false;
  }
  cin >> curr_line.input >> comma;
  cin >> curr_line.output;
  curr_line.is_valid = false;

  return true;
}

auto get_interval(const line &start, const line &end) -> interval {
  interval curr_interval;

  curr_interval.start = start.time;
  curr_interval.end = end.time;
  curr_interval.time = abs(end.time - start.time);
  curr_interval.signal = end.output;

  return curr_interval;
}

auto get_report(const interval &req, const interval &res, const report &last)
    -> report {
  report curr_report;

  curr_report.req = req;
  curr_report.res = res;
  curr_report.delay = abs(curr_report.res.start - curr_report.req.start);
  curr_report.jitter = curr_report.delay - last.delay;

  return curr_report;
}

void print_queue(queue<interval> intervals) {
  while (intervals.size()) {
    const auto res = intervals.front();
    cout << '#';
    print_interval(res);
    cout << endl;
    intervals.pop();
  }
}

void print_stats(queue<interval> reqs, queue<interval> resps, int cont_req,
                 int cont_resp) {
  cout << "#requests without answer:" << endl;
  print_queue(reqs);

  cout << "#not required responses:" << endl;
  print_queue(resps);

  cout << "#Total requests: " << cont_req << endl;
  cout << "#Total responses: " << cont_resp << endl;
}

auto find_interval(line &start, line &end, line &last, line &curr, bool input)
    -> bool {
  const bool last_signal = input ? last.input : last.output;
  const bool curr_signal = input ? curr.input : curr.output;

  if (!last_signal && curr_signal) {
    start = curr;
    start.is_valid = true;
  } else if (last_signal && !curr_signal) {
    end = last;
    end.is_valid = true;
  }

  return start.is_valid && end.is_valid;
}

void push_interval(int id, line &start, line &end, queue<interval> &intervals) {
  auto new_interval = get_interval(start, end);
  new_interval.id = id;
  intervals.push(new_interval);
  start.is_valid = false;
  end.is_valid = false;
}

void print_header() {
  cout << "req_id" << ',';
  cout << "req_start" << ',';
  cout << "req_end" << ',';
  cout << "req_duration" << ',';

  cout << "res_id" << ',';
  cout << "res_start" << ',';
  cout << "res_end" << ',';
  cout << "res_duration" << ',';

  cout << "delay" << ',';
  cout << "jitter" << endl;
}

int main() {

  std::cin.precision(16);
  std::cout.precision(16);

  vector<string> header = read_header(3);

  queue<interval> reqs;
  queue<interval> resps;

  line curr_line;
  line last_line;

  line req_start;
  line req_end;

  line res_start;
  line res_end;

  long long int cont_resp = 0;
  long long int cont_req = 0;
  report last_report;

  print_header();

  read_line(last_line);
  while (read_line(curr_line)) {

    const bool req_interval =
        find_interval(req_start, req_end, last_line, curr_line, true);

    const bool res_interval =
        find_interval(res_start, res_end, last_line, curr_line, false);

    if (res_interval) {
      push_interval(cont_resp, res_start, res_end, resps);
      cont_resp++;
    }

    if (req_interval) {
      push_interval(cont_req, req_start, req_end, reqs);
      cont_req++;
    }

    while (reqs.size() && resps.size()) {
      const auto req = reqs.front();
      const auto res = resps.front();
      const auto report = get_report(req, res, last_report);

      print_report(report);

      last_report = report;

      reqs.pop();
      resps.pop();
    }

    last_line = curr_line;
  }

  print_stats(reqs, resps, cont_req, cont_resp);

  return 0;
}
