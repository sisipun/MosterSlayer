#include "BaseCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

FName ABaseCharacter::WEAPON_SOCKET_NAME = TEXT("weapon");

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AttackAnimation = CreateDefaultSubobject<UAnimMontage>(TEXT("Attack Animation"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Mana = MaxMana;

	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::Hit);
	if (Weapons.Num() > 0) 
	{
		ChangeWeapon(0);
	}
}

void ABaseCharacter::Attack()
{
	if (!CurrentWeapon || bIsAttacking) 
	{
		return;
	}

	bIsAttacking = true;
	PlayAnimMontage(AttackAnimation);
}

void ABaseCharacter::BeginAttack()
{
	if (CurrentWeapon) 
	{
		CurrentWeapon->BeginAttack();
	}
}

void ABaseCharacter::EndAttack()
{
	if (CurrentWeapon) 
	{
		CurrentWeapon->EndAttack();
	}

	bIsAttacking = false;
}

void ABaseCharacter::Hit(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
}

bool ABaseCharacter::isDead()
{
	return Health <= 0;
}

bool ABaseCharacter::isAttacking()
{
	return bIsAttacking;
}


void ABaseCharacter::ChangeWeapon(int index)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}

	FTransform SocketTransform = GetMesh()->GetSocketTransform(ABaseCharacter::WEAPON_SOCKET_NAME);
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.Instigator = this;
	AActor* Weapon = GetWorld()->SpawnActor(Weapons[index], &SocketTransform, SpawnParams);
	CurrentWeapon = Cast<AWeapon>(Weapon);
	CurrentWeapon->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		),
		ABaseCharacter::WEAPON_SOCKET_NAME
	);
}

void ABaseCharacter::Destroyed()
{
	Super::Destroyed();

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
}
