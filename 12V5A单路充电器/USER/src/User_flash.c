#include "User_flash.h"


/*
项目组织
1.将eeprom.c添加到项目中
2.在设置菜单所在c文件中定义数组u16 FEE_data[]，并赋予初始值
3.修改eeprom.h中FEE_data数组大小及变量id枚举名
运用
4.上电初始化 FEE_init();
5.FEE_rd(en_feedata_t); // 读，数组标号用枚举名
   FEE_wr(u16,en_feedata_t); // 写
*/

#define FEE_LOCKS  (512*8) /* 锁定区字节数，一个锁定位管8页4096字节，以下地址须按此对齐*/
#define FEE_P0ADDR  0x0000F800  /* P0首地址 */
#define FEE_PAGES   1  /* 每P页数只能1页 */
#define FEE_WORDS  (512/4 * FEE_PAGES)  /* 字数 = 页字节数/4 *页数 */
#define FEE_P1ADDR (FEE_P0ADDR + FEE_WORDS*4) /* P1首地址 */


uint16_t FEE_data[5];//变量表初始值，数组标号对应变量ID。变量表和菜单参数放在一起

static uint32_t FEEcounts; // 累计擦除次数(用作启用页页首标记)
static uint32_t FEEaddr; // 当前页首地址，指向当前启用页
static uint16_t FEEoffset; // 页指针，指向当前页内可写地址

static void FEEeof(void); // 存满处理
static void FEEput(uint32_t addr); // 转存处理
static void FEEextr(uint32_t addr); // 提取数据
static void FEEerase(uint32_t addr); // 擦除
static void FEEerase_put(uint32_t addr); // 擦存处理
/***/
uint16_t FEE_rd(en_feedata_t id) // eeprom读
{
  return FEE_data[id]; // 直接读取数组成员
}
/***/
void FEE_wr(uint16_t data, en_feedata_t id) // eeprom写
{
  if(data!=FEE_data[id])
  {
    uint32_t addr;
    FEE_data[id]=data; // 更新数组成员

    addr=FEEaddr+FEEoffset*4; // 写FLASH地址
    CW_FLASH->PAGELOCK = 0x5A5A0000|(1<<(addr/FEE_LOCKS)); // 解锁地址所属锁定区
    while (CW_FLASH->ISR & 0x20); // 等待空闲
    CW_FLASH->CR1 = 0x5A5A0001; // 页编程模式
    *((uint32_t *)addr) = ((uint32_t)id)<<16|data; //id在高半字，值在低半字
    while (CW_FLASH->ISR & 0x20); // 等待完成
    CW_FLASH->CR1 = 0x5A5A0000; // 改为只读
    CW_FLASH->PAGELOCK = 0x5A5A0000; // 锁定页面
    FEEoffset++; // 指向下一个字
    FEEeof(); // 存满处理
  }
}
/***/
static void FEEput(uint32_t addr) // 转存
{
  CW_FLASH->PAGELOCK = 0x5A5A0000|(1<<(addr/FEE_LOCKS)); // 解锁地址所属锁定区，转存用到的地址必须都在同一个锁定区
  while (CW_FLASH->ISR & 0x20); // 等待空闲
  CW_FLASH->CR1 = 0x5A5A0001; // 页编程模式
  for(FEEoffset=1; FEEoffset<=(sizeof(FEE_data)/sizeof(FEE_data[0])); FEEoffset++) // 数组各成员
  {
    *((uint32_t *)(addr+FEEoffset*4)) = ((FEEoffset-1)<<16)|FEE_data[FEEoffset-1]; // id在高半字，值在低半字
    while (CW_FLASH->ISR & 0x20); // 等待完成
  }
  *((uint32_t *)addr) = ++FEEcounts; // 页首写累计擦除次数作为启用标记
  while (CW_FLASH->ISR & 0x20); // 等待完成
  CW_FLASH->CR1 = 0x5A5A0000; // 改为只读
  CW_FLASH->PAGELOCK = 0x5A5A0000; // 重新锁定
  FEEaddr=addr; // 作为当前页
}
/***/
static void FEEerase(uint32_t addr) // 擦除指定页面(只擦一页，每P有多页要改)
{
  CW_FLASH->PAGELOCK = 0x5A5A0000|(1<<(addr/FEE_LOCKS)); // 解锁地址所属锁定区
  while (CW_FLASH->ISR & 0x20); // 等待空闲
  CW_FLASH->CR1 = 0x5A5A0002; // 页擦除模式
  *((uint32_t *)addr) = 0; // 页面上写启动擦除
  while (CW_FLASH->ISR & 0x20); // 等待完成
  CW_FLASH->CR1 = 0x5A5A0000; // 改为只读
  CW_FLASH->PAGELOCK = 0x5A5A0000; // 重新锁定
}
/***/
static void FEEerase_put(uint32_t addr) // 擦存
{       
  FEEerase(addr); //先擦除
  FEEput(addr); //再转存
}
/***/
static void FEEextr(uint32_t addr) // 提取数据
{
  uint32_t data;
  FEEaddr=addr; //作为当前页
  for(FEEoffset=1; FEEoffset<FEE_WORDS; FEEoffset++) // 页首字是启用标记，从下一个字开始
  {
    data=*(uint32_t *)(addr+FEEoffset*4); // 字内容
    if(~data) // 非空，有数据
    {
      FEE_data[data>>16] = data & 0xFFFF;// 替换数组成员值
    }
    else // 空，数据区结束
    {
      break;
    }
  }               
}
/***/
static void FEEeof(void) // 存满处理
{
  if(FEEoffset>=FEE_WORDS) //指针到达字数
  {
    uint32_t temp=FEEaddr; // 当前页
    FEEput((temp==FEE_P0ADDR)?(FEE_P1ADDR):(FEE_P0ADDR)); // 转存到另一页
    FEEerase(temp); //擦除旧页               
  }
}



/***/
void FEE_init(void) // 上电初始化
{
  uint32_t temp,temp0,temp1;

  CW_SYSCTRL->AHBEN |= 0x5A5A0002; //开FLASH时钟

  temp=0;
  temp0 = *(uint32_t *)FEE_P0ADDR; //P0首字
  if(~temp0) temp|=1; // 非空(有0)
  if(~(*(uint32_t *)(FEE_P0ADDR + 4))) temp|=2; //P0数据非空
  temp1 = *(uint32_t *)(FEE_P1ADDR); // P1首字
  if(~temp1) temp|=4; // 非空
  if(~(*(uint32_t *)(FEE_P1ADDR + 4))) temp|=8; //P1数据非空
  switch (temp) /* 检查两个页面状态 */
  {
    case 0: case 2: // 0000:P0 P1全空 或 0010:P0初始化失败
      FEEcounts=1;
      FEEerase_put(FEE_P0ADDR); //擦存P0
      break;
    case 3: // 0011:P0启用中
      FEEcounts=temp0; // 累计擦除次数
      FEEextr(FEE_P0ADDR); //提取P0数据
      break;
    case 8: // 1011:P0>P1转存失败
      FEEcounts=temp0;
      FEEextr(FEE_P0ADDR); //提取P0数据
      FEEerase_put(FEE_P1ADDR); //擦存P1
      FEEerase(FEE_P0ADDR); //擦除P0
      break;
    case 12: // 1100:P1启用中
      FEEcounts=temp1;
      FEEextr(FEE_P1ADDR); //提取P1数据
      break;
    case 14: // 1110:P1>P0转存失败
      FEEcounts=temp1;
      FEEextr(FEE_P1ADDR); //提取P1数据
      FEEerase_put(FEE_P0ADDR); //擦存P0
      FEEerase(FEE_P1ADDR); //擦除P1
      break;
    case 15: // 1111:擦除失败
      if(temp0>temp1) // 标记数大的作为启用页
      {
        FEEcounts=temp0;
        FEEextr(FEE_P0ADDR); //提取P0数据
        FEEerase(FEE_P1ADDR); //擦除P1
      }
      else
      {
        FEEcounts=temp1;
        FEEextr(FEE_P1ADDR); //提取P1数据
        FEEerase(FEE_P0ADDR); //擦除P0
      }
      break;
  } /* 检查页面状态end */
  FEEeof(); // 满页处理
}
/***/
/*******************/













