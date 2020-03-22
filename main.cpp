#include <iostream>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

class Date {
public:
	Date(int new_year, int new_month, int new_day) {
		year = new_year;
		month = new_month;
		day = new_day;
	}
	int GetYear() const {
		return year;
	}
	int GetMonth() const {
		return month;
	}
	int GetDay() const {
		return day;
	}
private:
	int year;
	int month;
	int day;
};

bool operator<(const Date& lhs, const Date& rhs) {
	if (lhs.GetYear() == rhs.GetYear()) {
		if (lhs.GetMonth() == rhs.GetMonth()) {
			return lhs.GetDay() < rhs.GetDay();
		}
		return lhs.GetMonth() < rhs.GetMonth();
	}
	return lhs.GetYear() < rhs.GetYear();
}

void EnsureNextSymbolAndSkip(stringstream& stream) {
	if (stream.peek() != '-') {
		stringstream ss;
		throw exception();
	}
	stream.ignore(1);
}

void ReadNumber(stringstream& stream, int& number) {
	if (!(stream >> number)) {
		throw exception();
	}
}

Date ParseDate(const string& s) {
	stringstream stream(s);
	int year = 0, month = 0, day = 0;
	try {
		ReadNumber(stream, year);
		EnsureNextSymbolAndSkip(stream);
		ReadNumber(stream, month);
		EnsureNextSymbolAndSkip(stream);
		ReadNumber(stream, day);
		if (stream.peek() != EOF) {
			throw exception();
		}
	} catch (exception& ex) {
		throw invalid_argument("Wrong date format: " + s);
	}
	if (!(month >= 1 && month <= 12)) {
		throw invalid_argument("Month value is invalid: " + to_string(month));
	}
	if (!(day >= 1 && day <= 31)) {
		throw invalid_argument("Day value is invalid: " + to_string(day));
	}
	return {year, month, day};
}

class Database {
public:
	void AddEvent(const Date& date, const string& event) {
		events[date].insert(event);
	}
	bool DeleteEvent(const Date& date, const string& event) {
		if (events.count(date) == 1) {
			auto &events_by_date = events[date];
			if (count(begin(events_by_date), end(events_by_date), event) == 1) {
				events_by_date.erase(event);
				return true;
			}
		}
		return false;
	}
	int DeleteDate(const Date& date) {
		if (events.count(date) == 1) {
			int count = events[date].size();
			events.erase(date);
			return count;
		}
		return 0;
	}
	set<string> Find(const Date& date) const {
		if (events.count(date) == 1) {
			return events.at(date);
		}
		return {};
	}
	void Print() const {
		for (const auto& item : events) {
			const Date& date = item.first;
			for (auto& event : item.second) {
				cout << setw(4) << setfill('0') << date.GetYear() << '-'
						 << setw(2) << setfill('0') << date.GetMonth() << '-'
						 << setw(2) << setfill('0') << date.GetDay() << ' '
						 << event << endl;
			}
		}
	}
private:
	map<Date, set<string>> events;
};

int main() {
	Database db;
	string command;
	while (getline(cin, command)) {
		stringstream stream_str(command);
		string operation_code;
		stream_str >> operation_code;
		try {
			if (operation_code == "Add") {
				string date_str, event;
				stream_str >> date_str >> event;
				Date date = ParseDate(date_str);
				db.AddEvent(date, event);
			} else if (operation_code == "Del") {
				string date_str, event;
				stream_str >> date_str;
				Date date = ParseDate(date_str);
				if (stream_str >> event) {
					if (db.DeleteEvent(date, event)) {
						cout << "Deleted successfully" << endl;
					} else {
						cout << "Event not found" << endl;
					}
				} else {
					cout << "Deleted " << db.DeleteDate(date) << " events" << endl;
				}
			} else if (operation_code == "Find") {
				string date_str;
				stream_str >> date_str;
				Date date = ParseDate(date_str);
				set<string> events_by_date = db.Find(date);
				for (const auto &event : events_by_date) {
					cout << event << endl;
				}
			} else if (operation_code == "Print") {
				db.Print();
			} else if (!operation_code.empty()) {
				cout << "Unknown command: " << command << endl;
			}
		} catch (exception& ex) {
			cout << ex.what() << endl;
		}
	}
	return 0;
}
