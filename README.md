# CPU-Scheduler
模擬CPU排程演算法，包括FCFS(先進先出)、RR(知更鳥式循環)、 SRTF(最短工作優先)、 PPRR、 HRRN(反應時間比率優先等級)。

# 開發環境 :
macOS Monterey
# 使用開發環境 : 
Xcode Version 13.2.1 (13C100) 
# 使用的程式語言 : C++
# 實作方法與流程
#### 實作方法:
1. 將檔案打開後先讀入方法、time_slice後，再 依序讀入 ID, CPU Burst, Arrival Time, Priority，存入 vector<pData> ALLprocess; pData 為 struct，包含所有所需資料。
2. 先把所有資料依照ArrivalTime由小到大排 序，若 Arrival Time 相同，則由 ID 小的排在 前面。
3. 再依照讀入的方法，判斷要做何種處理原則。
* 方法一: FCFS，

採取先到先處理的方式，每次取出排序好的第0筆來做處理，並加入甘特圖。
直到此 process 完全做完後，計算 Turnaround, Waiting Time 後，加入輸出的陣列，再取出下一筆，依此類推。
* 方法二: RR，

採取每個process只能使用一個time_slice的時間，用完 則換下一個 process 做處理。如果有小於目前時間的 processes，加入到 queue 中排序，若在此 time_slice 將此 process 完   全做完後，計算 Turnaround, Waiting Time 後，加入輸出的陣列;在一個 time_slice 中 尚未完成此 process，則加入到 queue 最後面。若做完的此時間有新的 process 到達，則把新的 process 排在最前面，依此類推。
* 方法三: SRTF，

採取剩餘CpuBurst最小的process先做處理。將小 於目前時間的 processes 加入陣列，每次要做 process 處理挑選時，先 把陣列各 process 的 Cpu Burst 由小到大排序，若相同，則比較 Arrival Time 再比較 ID，每次取出第 0 筆做處理，若有比此 process 更 小的 Cpu Burst 則換成其 process 做處理，且此 process 尚未做完則加入到 queue 中等待排序;若已做完，計算 Turnaround, Waiting Time後，加入輸出的陣列，以此類推。
* 方法四: PPRR，

採取將priority最小的process優先處理，若有多個相
同 priority 的 processes，則進行 RR 排程。將小於目前時間的 processes 加入陣列，每次要做 process 處理挑選時，先把陣列各 process 的 priority 由小到大排序，若只有一個 process，直接進行處理，並蕤時判 斷是否出現 priority 更小的 process，則必須讓出 cpu，並重新添加回 queue;如果有多個 processes 則加入 RRqueue 中，直至此 time_slice 做 完，才會下一個，除非有比目前 priority 更小的 process 出現，則立刻 換之處理。若已做完，計算 Turnaround, Waiting Time 後，加入輸出 的陣列，以此類推。
* 方法五: HRRN，

採取反應時間比率 (waiting time + cpu brust) / cpu brust 較高的 process 先處理，將小於目前時間的 processes 加入陣列，每輪挑選前 先依照反應時間比率做排序，挑選最高的 process 做處理，直至結束才 換下個 process，做完，計算 Turnaround, Waiting Time 後，加入輸出 的陣列，以此類推。
* 方法六:為執行1-6方法後的結果。
