
#!/bin/bash

# 统计总共有多少个文件小于10000行
count=0
total_lines=0
file_count=0

# 统计每一个以"log"开头的文件的行数
for file in ./build/log/log*; do
    if [ -f "$file" ]; then
        lines=$(wc -l < "$file")
        total_lines=$((total_lines + lines))
        file_count=$((file_count + 1))
        if [ "$lines" -lt 10000 ]; then
            count=$((count + 1))
        fi
    fi
done

# 计算平均行数
if [ "$file_count" -gt 0 ]; then
    average_lines=$((total_lines / file_count))
else
    average_lines=0
fi

echo "Total number of files with less than 10000 lines: $count"
echo "Average number of lines in all files: $average_lines"