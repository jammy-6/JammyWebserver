
#!/bin/bash

# 统计每一个以"log"开头的文件的行数
for file in ./build/log/log*; do
    if [ -f "$file" ]; then
         lines=$(wc -l < "$file")
          echo "File $file has $lines lines."
     fi
done