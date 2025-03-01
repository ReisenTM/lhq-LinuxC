#!/bin/bash

# 设置程序路径
PROGRAM="./client"

# 设置运行次数
COUNT=0

# 默认参数 127.0.0.1
PARAMS="127.0.0.1"

# 循环执行程序
while true; do
  # 增加计数
  COUNT=$((COUNT + 1))

  # 执行程序并传入参数
  echo "执行第 $COUNT 次，使用参数：$PARAMS..."
  $PROGRAM $PARAMS

  # 记录每次执行的次数，可以将其保存到日志文件中
  echo "第 $COUNT 次执行完成" >>execution_log.txt

  # 可选：根据需要添加延迟（比如每次执行后等待5秒）
  sleep 1
done
