#include "analyzer.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

void TripAnalyzer::ingestFile(string csvPath) {
    ifstream file(csvPath);
    if (!file.is_open()) return;
    string line;
    if (!getline(file, line)) return;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t c1 = line.find(','); if (c1 == string::npos) continue;
        size_t c2 = line.find(',', c1 + 1); if (c2 == string::npos) continue;
        size_t c3 = line.find(',', c2 + 1); if (c3 == string::npos) continue;
        size_t c4 = line.find(',', c3 + 1); if (c4 == string::npos) continue;
        size_t c5 = line.find(',', c4 + 1); if (c5 == string::npos) continue;

        string zoneId = line.substr(c1 + 1, c2 - c1 - 1);
        if (zoneId.empty()) continue;
        string pickupTime = line.substr(c3 + 1, c4 - c3 - 1);
        if (pickupTime.length() < 13) continue;

        char h0 = pickupTime[11], h1 = pickupTime[12];
        if (!isdigit((unsigned char)h0) || !isdigit((unsigned char)h1)) continue;
        int hour = (h0 - '0') * 10 + (h1 - '0');
        if (hour < 0 || hour > 23) continue;

        zoneCounts[zoneId]++;
        slotCounts[zoneId + "#" + to_string(hour)]++;
    }
}

void TripAnalyzer::ingestStdin() {
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        size_t c1 = line.find(','); if (c1 == string::npos) continue;
        size_t c2 = line.find(',', c1 + 1); if (c2 == string::npos) continue;
        size_t c3 = line.find(',', c2 + 1); if (c3 == string::npos) continue;
        size_t c4 = line.find(',', c3 + 1); if (c4 == string::npos) continue;
        size_t c5 = line.find(',', c4 + 1); if (c5 == string::npos) continue;

        string zoneId = line.substr(c1 + 1, c2 - c1 - 1);
        if (zoneId.empty()) continue;
        string pickupTime = line.substr(c3 + 1, c4 - c3 - 1);
        if (pickupTime.length() < 13) continue;

        char h0 = pickupTime[11], h1 = pickupTime[12];
        if (!isdigit((unsigned char)h0) || !isdigit((unsigned char)h1)) continue;
        int hour = (h0 - '0') * 10 + (h1 - '0');
        if (hour < 0 || hour > 23) continue;

        zoneCounts[zoneId]++;
        slotCounts[zoneId + "#" + to_string(hour)]++;
    }
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> v;
    for (auto const& p : zoneCounts) v.push_back({ p.first, p.second });
    sort(v.begin(), v.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
        });
    if ((int)v.size() > k) v.resize(k);
    return v;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> v;
    for (auto const& p : slotCounts) {
        size_t pos = p.first.find('#');
        if (pos == string::npos) continue;
        v.push_back({ p.first.substr(0, pos), stoi(p.first.substr(pos + 1)), p.second });
    }
    sort(v.begin(), v.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
        });
    if ((int)v.size() > k) v.resize(k);
    return v;
}