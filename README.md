# CPU-Scheduler
模擬CPU排程演算法，包括FCFS(先進先出)、RR(知更鳥式循環)、 SRTF(最短工作優先)、 PPRR、 HRRN(反應時間比率優先等級)。

# 開發環境 :
macOS Monterey
# 使用開發環境 : 
Xcode Version 13.2.1 (13C100) 
# 使用的程式語言 : 
C++
# 實作方法與流程
#### 流程:
<img width="378" alt="截圖 2022-10-07 下午6 05 04" src="https://user-images.githubusercontent.com/95215851/194529436-2d00d96b-9de3-4ef3-9d86-2c4eb9584786.png">

1. 根據使用者的檔名先去判斷檔案是否存在，若不存在則再輸入一次， 反之則進行下一步驟。
2. 成功開啟檔案後，讀取檔案第一行的數字，分別紀錄為全域變數的處 理方法及 time_slice。
3. 再依序讀入所需資料且存入 vector<pData> ALLprocess;
#### 實作方法:
1. 將檔案打開後先讀入方法、time_slice後，再 依序讀入 ID, CPU Burst, Arrival Time, Priority，存入 vector。
2. 先把所有資料依照ArrivalTime由小到大排 序，若 Arrival Time 相同，則由 ID 小的排在 前面。
3. 再依照讀入的方法，判斷要做何種處理原則。
4. 輸出檔案
* 方法一: FCFS

採取先到先處理的方式，每次取排序好的第0筆來做處理，並加入甘特圖。
直到此 process 完全做完後，計算 Turnaround, Waiting Time 後，加入輸出的陣列，再取出下一筆，依此類推。
* 方法二: RR

採取每個process只能使用一個time_slice的時間，用完 則換下一個 process 做處理。如果有小於目前時間的 processes，加入到 queue 中排序，若在此 time_slice 將此 process 完   全做完後，計算 Turnaround, Waiting Time 後，加入輸出的陣列;在一個 time_slice 中 尚未完成此 process，則加入到 queue 最後面。若做完的此時間有新的 process 到達，則把新的 process 排在最前面，依此類推。
* 方法三: SRTF

採取剩餘CpuBurst最小的process先做處理。將小 於目前時間的 processes 加入陣列，每次要做 process 處理挑選時，先 把陣列各 process 的 Cpu Burst 由小到大排序，若相同，則比較 Arrival Time 再比較 ID，每次取出第 0 筆做處理，若有比此 process 更 小的 Cpu Burst 則換成其 process 做處理，且此 process 尚未做完則加入到 queue 中等待排序;若已做完，計算 Turnaround, Waiting Time後，加入輸出的陣列，以此類推。
* 方法四: PPRR

採取將priority最小的process優先處理，若有多個相
同 priority 的 processes，則進行 RR 排程。將小於目前時間的 processes 加入陣列，每次要做 process 處理挑選時，先把陣列各 process 的 priority 由小到大排序，若只有一個 process，直接進行處理，並蕤時判 斷是否出現 priority 更小的 process，則必須讓出 cpu，並重新添加回 queue;如果有多個 processes 則加入 RRqueue 中，直至此 time_slice 做 完，才會下一個，除非有比目前 priority 更小的 process 出現，則立刻 換之處理。若已做完，計算 Turnaround, Waiting Time 後，加入輸出 的陣列，以此類推。
* 方法五: HRRN

採取反應時間比率 (waiting time + cpu brust) / cpu brust 較高的 process 先處理，將小於目前時間的 processes 加入陣列，每輪挑選前 先依照反應時間比率做排序，挑選最高的 process 做處理，直至結束才 換下個 process，做完，計算 Turnaround, Waiting Time 後，加入輸出 的陣列，以此類推。
* 方法六:
  
為執行1-6方法後的結果。
  
# 不同排程法的比較:
<img width="677" alt="截圖 2022-10-07 下午5 55 45" src="https://user-images.githubusercontent.com/95215851/194527337-ad1b17d1-3848-4c79-8245-4f2d83252eab.png">
<img width="660" alt="截圖 2022-10-07 下午5 55 53" src="https://user-images.githubusercontent.com/95215851/194527355-c1be2710-b1c3-466f-bf73-9e9adff9f087.png">
依照上圖及各排程方法所採取的特性，可以得知以下:
  
1. FCFS: 由於是先到先處理，所以不會產生 Starvation，但可以看出排程效益並不好，可能會造成護航效果(convoy effect) ，即為很多 Processes 均在等待一個需要很長 CPU Time 來完成工作的Process，造成平均等待時間大幅增加的情況發生，且平均等待時間,往返時間增加。
  
2. RR: 為分時系統(Time-Sharing)設計，每個 process 分配一個 time slice，允許 process 在時間段中執行，時間一到就分配給另外一個 process。也因此不會發生 Starvation，但是根據time slice 的長短可能會影響排程效率。
* 時間片段太大➔類似於先到先服務排程法。ex: input3 的 time_slice 為 10。
* 時間片段太小➔系統的效率太差(context switch 時間被佔滿， process 根本無法 run 或 run 極短時間)
    
ex: input1 的 time_slice 為 1 造成系統效率較差。影響平均等待時間,往返時間。
  
3. SRFT: 由剩餘 CPU Brust 最小的 process 先執行。因此效益最佳: 平均 waiting time, Turnaround Time 最少，但是無法得知下一個 CPU 區間長度且如果為需要很長時間來執行的 process 就會一直被 無限推遲，造成 starvation，但可利用的解決辦法:Aging。
  
4. PPRR: 為先採用高優先權的程序會最先被分配到 CPU，相同優先 權則進行 RR，優先權排程可能會造成低優先權的 process starvation，並且可能也會擁有 RR 的 time_slice 問題，由於有些 processes 採用優先權排程，因此可以不用全部 processes 都在輪流 執行，因此由圖可知，平均等待時間、往返時間比單純做 RR 排程的少。
  
5. HRRN: 為動態調整優先順序的演算法，由於作業的服務時間是固 定的，排程優先順序隨著已等待時間的提高而變大，用來避免 Starvation 的情況發生，而導致平均等待時間,往返時間過大。

# input 說明
  <img width="313" alt="截圖 2022-10-07 下午6 05 16" src="https://user-images.githubusercontent.com/95215851/194529466-bf71476c-6fa3-4d15-b66b-ec538a612e72.png">
  第一行2個數字代表: 使用方法、time_slice。
  第三行開始包含四個數字依序為ID, CPU Brust, Arrival time, Priority
  

# output 說明
  <img width="415" alt="截圖 2022-10-07 下午6 07 43" src="https://user-images.githubusercontent.com/95215851/194529575-7c5b72a4-d93d-4a03-a2c1-697939f5c322.png">
依序輸出甘特圖、各process之waiting time、各process之 turnaround time

