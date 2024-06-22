import asyncio

async def timer(sec):
    while True:
        print(f"sleep for {sec} seconds!")
        await asyncio.sleep(sec)

async def echo():
    while True:
        print(await asyncio.to_thread(input));

async def main():
    async with asyncio.TaskGroup() as tg:
        tg.create_task(timer(3));
        tg.create_task(echo());

asyncio.run(main())

