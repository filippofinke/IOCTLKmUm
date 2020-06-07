#include <wdm.h>

UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING(L"\\Device\\IOCTLKmUm");
UNICODE_STRING DEVICE_SYMBOLIC_NAME = RTL_CONSTANT_STRING(L"\\DosDevices\\IOCTLKmUm");

NTSTATUS OnMessage(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PCHAR welcome = "IOCTLKmUm - Hello from kernel!";
	PVOID pBuf = Irp->AssociatedIrp.SystemBuffer;
	PIO_STACK_LOCATION pIoStackLocation = IoGetCurrentIrpStackLocation(Irp);

	if (pIoStackLocation->Parameters.DeviceIoControl.IoControlCode == 0x800) {
		DbgPrint("IOCTLKmUm - Received: %s\n", pBuf);
		RtlZeroMemory(pBuf, pIoStackLocation->Parameters.DeviceIoControl.InputBufferLength);
		RtlCopyMemory(pBuf, welcome, strlen(welcome));
		Irp->IoStatus.Information = strlen(welcome);
	}
	else {
		Irp->IoStatus.Information = 0;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS OnCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("IOCTLKmUm - Handle opened: %wZ\n", DEVICE_SYMBOLIC_NAME);
	return STATUS_SUCCESS;
}

NTSTATUS OnClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	DbgPrint("IOCTLKmUm - Hanlde closed: %wZ\n", DEVICE_SYMBOLIC_NAME);
	return STATUS_SUCCESS;
}


void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	DbgPrint("IOCTLKmUm - Device deleted!\n");
	IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrint("IOCTLKmUm - SymbolicLink deleted!\n");
	IoDeleteSymbolicLink(&DEVICE_SYMBOLIC_NAME);
	DbgPrint("IOCTLKmUm - Driver unloaded!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS status = 0;

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OnMessage;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = OnCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = OnClose;

	DbgPrint("IOCTLKmUm - Driver loaded\n");

	status = IoCreateDevice(DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DriverObject->DeviceObject);
	if (NT_SUCCESS(status))
		DbgPrint("IOCTLKmUm - Device created: %wZ\n", DEVICE_NAME);
	else
		DbgPrint("IOCTLKmUm - Error creating device: %wZ\n", DEVICE_NAME);

	status = IoCreateSymbolicLink(&DEVICE_SYMBOLIC_NAME, &DEVICE_NAME);
	if (NT_SUCCESS(status))
		DbgPrint("IOCTLKmUm - Symbolic link created: %wZ\n", DEVICE_SYMBOLIC_NAME);
	else
		DbgPrint("IOCTLKmUm - Error creating symbolic link: %wZ\n", DEVICE_SYMBOLIC_NAME);

	return STATUS_SUCCESS;
}
