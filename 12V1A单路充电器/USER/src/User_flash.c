#include "User_flash.h"


/*
��Ŀ��֯
1.��eeprom.c��ӵ���Ŀ��
2.�����ò˵�����c�ļ��ж�������u16 FEE_data[]���������ʼֵ
3.�޸�eeprom.h��FEE_data�����С������idö����
����
4.�ϵ��ʼ�� FEE_init();
5.FEE_rd(en_feedata_t); // ������������ö����
   FEE_wr(u16,en_feedata_t); // д
*/

#define FEE_LOCKS  (512*8) /* �������ֽ�����һ������λ��8ҳ4096�ֽڣ����µ�ַ�밴�˶���*/
#define FEE_P0ADDR  0x0000F800  /* P0�׵�ַ */
#define FEE_PAGES   1  /* ÿPҳ��ֻ��1ҳ */
#define FEE_WORDS  (512/4 * FEE_PAGES)  /* ���� = ҳ�ֽ���/4 *ҳ�� */
#define FEE_P1ADDR (FEE_P0ADDR + FEE_WORDS*4) /* P1�׵�ַ */


uint16_t FEE_data[5];//�������ʼֵ�������Ŷ�Ӧ����ID��������Ͳ˵���������һ��

static uint32_t FEEcounts; // �ۼƲ�������(��������ҳҳ�ױ��)
static uint32_t FEEaddr; // ��ǰҳ�׵�ַ��ָ��ǰ����ҳ
static uint16_t FEEoffset; // ҳָ�룬ָ��ǰҳ�ڿ�д��ַ

static void FEEeof(void); // ��������
static void FEEput(uint32_t addr); // ת�洦��
static void FEEextr(uint32_t addr); // ��ȡ����
static void FEEerase(uint32_t addr); // ����
static void FEEerase_put(uint32_t addr); // ���洦��
/***/
uint16_t FEE_rd(en_feedata_t id) // eeprom��
{
  return FEE_data[id]; // ֱ�Ӷ�ȡ�����Ա
}
/***/
void FEE_wr(uint16_t data, en_feedata_t id) // eepromд
{
  if(data!=FEE_data[id])
  {
    uint32_t addr;
    FEE_data[id]=data; // ���������Ա

    addr=FEEaddr+FEEoffset*4; // дFLASH��ַ
    CW_FLASH->PAGELOCK = 0x5A5A0000|(1<<(addr/FEE_LOCKS)); // ������ַ����������
    while (CW_FLASH->ISR & 0x20); // �ȴ�����
    CW_FLASH->CR1 = 0x5A5A0001; // ҳ���ģʽ
    *((uint32_t *)addr) = ((uint32_t)id)<<16|data; //id�ڸ߰��֣�ֵ�ڵͰ���
    while (CW_FLASH->ISR & 0x20); // �ȴ����
    CW_FLASH->CR1 = 0x5A5A0000; // ��Ϊֻ��
    CW_FLASH->PAGELOCK = 0x5A5A0000; // ����ҳ��
    FEEoffset++; // ָ����һ����
    FEEeof(); // ��������
  }
}
/***/
static void FEEput(uint32_t addr) // ת��
{
  CW_FLASH->PAGELOCK = 0x5A5A0000|(1<<(addr/FEE_LOCKS)); // ������ַ������������ת���õ��ĵ�ַ���붼��ͬһ��������
  while (CW_FLASH->ISR & 0x20); // �ȴ�����
  CW_FLASH->CR1 = 0x5A5A0001; // ҳ���ģʽ
  for(FEEoffset=1; FEEoffset<=(sizeof(FEE_data)/sizeof(FEE_data[0])); FEEoffset++) // �������Ա
  {
    *((uint32_t *)(addr+FEEoffset*4)) = ((FEEoffset-1)<<16)|FEE_data[FEEoffset-1]; // id�ڸ߰��֣�ֵ�ڵͰ���
    while (CW_FLASH->ISR & 0x20); // �ȴ����
  }
  *((uint32_t *)addr) = ++FEEcounts; // ҳ��д�ۼƲ���������Ϊ���ñ��
  while (CW_FLASH->ISR & 0x20); // �ȴ����
  CW_FLASH->CR1 = 0x5A5A0000; // ��Ϊֻ��
  CW_FLASH->PAGELOCK = 0x5A5A0000; // ��������
  FEEaddr=addr; // ��Ϊ��ǰҳ
}
/***/
static void FEEerase(uint32_t addr) // ����ָ��ҳ��(ֻ��һҳ��ÿP�ж�ҳҪ��)
{
  CW_FLASH->PAGELOCK = 0x5A5A0000|(1<<(addr/FEE_LOCKS)); // ������ַ����������
  while (CW_FLASH->ISR & 0x20); // �ȴ�����
  CW_FLASH->CR1 = 0x5A5A0002; // ҳ����ģʽ
  *((uint32_t *)addr) = 0; // ҳ����д��������
  while (CW_FLASH->ISR & 0x20); // �ȴ����
  CW_FLASH->CR1 = 0x5A5A0000; // ��Ϊֻ��
  CW_FLASH->PAGELOCK = 0x5A5A0000; // ��������
}
/***/
static void FEEerase_put(uint32_t addr) // ����
{       
  FEEerase(addr); //�Ȳ���
  FEEput(addr); //��ת��
}
/***/
static void FEEextr(uint32_t addr) // ��ȡ����
{
  uint32_t data;
  FEEaddr=addr; //��Ϊ��ǰҳ
  for(FEEoffset=1; FEEoffset<FEE_WORDS; FEEoffset++) // ҳ���������ñ�ǣ�����һ���ֿ�ʼ
  {
    data=*(uint32_t *)(addr+FEEoffset*4); // ������
    if(~data) // �ǿգ�������
    {
      FEE_data[data>>16] = data & 0xFFFF;// �滻�����Աֵ
    }
    else // �գ�����������
    {
      break;
    }
  }               
}
/***/
static void FEEeof(void) // ��������
{
  if(FEEoffset>=FEE_WORDS) //ָ�뵽������
  {
    uint32_t temp=FEEaddr; // ��ǰҳ
    FEEput((temp==FEE_P0ADDR)?(FEE_P1ADDR):(FEE_P0ADDR)); // ת�浽��һҳ
    FEEerase(temp); //������ҳ               
  }
}



/***/
void FEE_init(void) // �ϵ��ʼ��
{
  uint32_t temp,temp0,temp1;

  CW_SYSCTRL->AHBEN |= 0x5A5A0002; //��FLASHʱ��

  temp=0;
  temp0 = *(uint32_t *)FEE_P0ADDR; //P0����
  if(~temp0) temp|=1; // �ǿ�(��0)
  if(~(*(uint32_t *)(FEE_P0ADDR + 4))) temp|=2; //P0���ݷǿ�
  temp1 = *(uint32_t *)(FEE_P1ADDR); // P1����
  if(~temp1) temp|=4; // �ǿ�
  if(~(*(uint32_t *)(FEE_P1ADDR + 4))) temp|=8; //P1���ݷǿ�
  switch (temp) /* �������ҳ��״̬ */
  {
    case 0: case 2: // 0000:P0 P1ȫ�� �� 0010:P0��ʼ��ʧ��
      FEEcounts=1;
      FEEerase_put(FEE_P0ADDR); //����P0
      break;
    case 3: // 0011:P0������
      FEEcounts=temp0; // �ۼƲ�������
      FEEextr(FEE_P0ADDR); //��ȡP0����
      break;
    case 8: // 1011:P0>P1ת��ʧ��
      FEEcounts=temp0;
      FEEextr(FEE_P0ADDR); //��ȡP0����
      FEEerase_put(FEE_P1ADDR); //����P1
      FEEerase(FEE_P0ADDR); //����P0
      break;
    case 12: // 1100:P1������
      FEEcounts=temp1;
      FEEextr(FEE_P1ADDR); //��ȡP1����
      break;
    case 14: // 1110:P1>P0ת��ʧ��
      FEEcounts=temp1;
      FEEextr(FEE_P1ADDR); //��ȡP1����
      FEEerase_put(FEE_P0ADDR); //����P0
      FEEerase(FEE_P1ADDR); //����P1
      break;
    case 15: // 1111:����ʧ��
      if(temp0>temp1) // ����������Ϊ����ҳ
      {
        FEEcounts=temp0;
        FEEextr(FEE_P0ADDR); //��ȡP0����
        FEEerase(FEE_P1ADDR); //����P1
      }
      else
      {
        FEEcounts=temp1;
        FEEextr(FEE_P1ADDR); //��ȡP1����
        FEEerase(FEE_P0ADDR); //����P0
      }
      break;
  } /* ���ҳ��״̬end */
  FEEeof(); // ��ҳ����
}
/***/
/*******************/













