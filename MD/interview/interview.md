<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [C++](#c)
	- [Overloading과 Overriding의 차이](#overloading-overriding-)
	- [다형성에 대해 설명하시오](#-)
	- [그럼 이건요](#-)

<!-- /TOC -->
# C++

## Overloading과 Overriding의 차이
Overloading
- 함수의 이름은 같으나 반환 타입이나 인자의 갯수, 인자의 타입이 다른경우 함수를 재정의 하는것

```c
int sum(int a, int b)
{
    return a + b;
}

int sum(int a, int b, int c) //overloading
{
    return a + b + c;
}
```

Overriding
- 상속의 관계에서 부모와 자식의 함수(메서드)가 같은 기능을 수행하지만 함수의 내용이 다른 경우 자식의 클래스에서 함수를 재정의 하는것.
- 함수(메서드)의 이름, return type, 매개변수가 같아야 함
- 접근 제한자의 범위가 부모와 같거나 더 넓어야 함
  - ex) 부모 - protected, 자식 - public >> O<br>
      부모 - public, 자식 - protected >> X

```c
//Child가 Parent를 상속 받았다고 가정.
void Parent :: whoAmI()
{
    printf("부모입니다.\n");
}

void Child :: whoAmI() // overriding
{
    printf("자식입니다.\n");
}
```

## 다형성에 대해 설명하시오
ㄴㅁㅇㅁㄴㅇ<br><br><br><br><br><br><br><br><br><br><br><br>


## 그럼 이건요
<br><br><br><br><br><br><br><br><br><br><br><br>
