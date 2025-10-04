// 2025.9.14 by CHEN_HONGTIAN MUST
// PERT model for software engineering project
// 以下是头文件pert.h

#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>

// 构建数据结构
struct Activity
{
    std::string activity_id;               // 活动 ID
    float duration = 0.0f;                 // 工期
    std::vector<std::string> predecessors; // 前驱
    std::vector<std::string> successors;   // 后继

    float ES = 0.0f;
    float EF = 0.0f;
    float LS = 0.0f;
    float LF = 0.0f;
    float slack = 0.0f; // slack = LS - ES
};

// 构建类
class PERT
{
public:
    PERT() = default;
    ~PERT() = default;

    // 添加活动(前驱可为NULL). 返回 false 表示重复 ID 或非法参数
    bool addActivity(const std::string &id,
                     float duration,
                     const std::vector<std::string> &predecessors);

    // 构建图 -> 拓扑排序 -> 正/反向计算 -> slack
    bool calculate();

    // 输出所有活动信息
    void printActivities() const;

    // 关键路径
    std::optional<std::vector<std::string>> getCriticalPath() const;

    // 总工期
    float getProjectDuration() const { return project_duration; }

    // 工具: 查找活动(只读), 找不到返回 nullptr
    const Activity *find(const std::string &id) const;

    // 清空所有数据
    void clear();

private:
    std::map<std::string, Activity> activities; // 活动集合
    float project_duration = 0.0f;              // 项目总工期
    std::vector<std::string> topo_order;        // 拓扑顺序缓存
    std::vector<std::string> critical_path;     // 计算后缓存关键路径

    // 构建 successors 并拓扑排序
    bool buildGraph(std::string &err);
    bool topoSort(std::string &err);

    // 计算阶段
    bool forwardPass(); // 正向计算
    bool backwardPass();// 反向计算
    void calculateSlack();// 计算 slack
    void deriveCriticalPath();// 计算关键路径
};