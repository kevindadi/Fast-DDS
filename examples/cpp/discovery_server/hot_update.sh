#!/bin/bash

# Usage: ./hot_update.sh <old_publisher_pid> [new_publisher_binary] [publisher_args...]

set -e

OLD_PID=$1
NEW_BINARY=${2:-./build/ds_publisher}

# 获取 Publisher 的额外参数（从第 3 个参数开始）
shift 2 2>/dev/null || shift $#
PUBLISHER_ARGS="$@"

if [ -z "$OLD_PID" ]; then
    echo "用法: $0 <old_publisher_pid> [new_publisher_binary] [publisher_args...]"
    echo "示例: $0 12345"
    echo "示例: $0 12345 ./build/ds_publisher"
    echo "示例: $0 12345 ./build/ds_publisher -r --transient-local"
    exit 1
fi

if ! ps -p $OLD_PID > /dev/null 2>&1; then
    echo "错误: 进程 $OLD_PID 不存在"
    exit 1
fi

if [ ! -f "$NEW_BINARY" ]; then
    echo "错误: 新版本文件 $NEW_BINARY 不存在"
    exit 1
fi

echo "=========================================="
echo "旧版本 PID: $OLD_PID"
echo "新版本: $NEW_BINARY"
echo ""

echo "[1/4] 启动新版本 Publisher (优先级=15)..."
if [ -n "$PUBLISHER_ARGS" ]; then
    echo "Publisher 参数: $PUBLISHER_ARGS"
    PUBLISHER_STRENGTH=15 $NEW_BINARY $PUBLISHER_ARGS &
else
    PUBLISHER_STRENGTH=15 $NEW_BINARY &
fi
NEW_PID=$!
echo "新版本 PID: $NEW_PID"

echo ""
echo "[2/4] 等待新版本连接到 Discovery Server (5秒)..."
sleep 5

if ! ps -p $NEW_PID > /dev/null 2>&1; then
    echo "错误: 新版本启动失败！"
    exit 1
fi
echo "新版本运行正常"

echo ""
echo "[3/4] 发送 SIGTERM 触发旧版本优雅关闭..."
kill -TERM $OLD_PID

echo ""
echo "[4/4] 等待旧版本完全退出..."
TIMEOUT=15
ELAPSED=0
while ps -p $OLD_PID > /dev/null 2>&1; do
    if [ $ELAPSED -ge $TIMEOUT ]; then
        echo "警告: 旧版本未在 ${TIMEOUT}秒内退出，强制终止"
        kill -9 $OLD_PID
        break
    fi
    sleep 1
    ELAPSED=$((ELAPSED + 1))
    echo -ne "等待... ${ELAPSED}s\r"
done

echo ""
echo "新版本 PID: $NEW_PID"
echo "状态文件: /tmp/fastdds_publisher_state.txt"
echo "=========================================="

# 显示状态文件内容
if [ -f /tmp/fastdds_publisher_state.txt ]; then
    echo "当前状态:"
    cat /tmp/fastdds_publisher_state.txt
fi

