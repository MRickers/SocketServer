#include <catch2/catch_test_macros.hpp>

#include <server/thread_pool/thread_pool.h>
#include <string>

TEST_CASE("Push/Pop string to queue", "[queue]") {
    using namespace socket_server;
    MutexQueue<std::string> q;

    SECTION("Push one string to q") {
        
        REQUIRE_NOTHROW(q.Push("foo"));
        REQUIRE(q.Pop().value() == "foo");
    }

    SECTION("Push five strings to q") {
        REQUIRE_NOTHROW(q.Push("bar"));
        REQUIRE_NOTHROW(q.Push("baz"));
        REQUIRE_NOTHROW(q.Push("foo"));
        REQUIRE_NOTHROW(q.Push("faz"));
        REQUIRE_NOTHROW(q.Push("foobar"));

        REQUIRE(q.Pop().value() == "bar");
        REQUIRE(q.Pop().value() == "baz");
        REQUIRE(q.Pop().value() == "foo");
        REQUIRE(q.Pop().value() == "faz");
        REQUIRE(q.Pop().value() == "foobar");
    }
}

TEST_CASE("Copy queue", "[queue]") {
    using namespace socket_server;
    MutexQueue<int> q;

    SECTION("Copy constructor") {
        q.Push(1);
        q.Push(2);

        MutexQueue<int> q2(q);

        REQUIRE(q2.Pop().value() == 1);
        REQUIRE(q2.Pop().value() == 2);
    }

    SECTION("Copy assignment constructor") {
        q.Push(3);
        q.Push(4);

        MutexQueue<int> q2 = q;

        REQUIRE(q2.Pop().value() == 3);
        REQUIRE(q2.Pop().value() == 4);
    }

    SECTION("Assignment constructor") {
        q.Push(1337);
        q.Push(123456);
        q.Push(99);

        MutexQueue<int> q2;
        q2 = q;

        REQUIRE(q2.Pop().value() == 1337);
        REQUIRE(q2.Pop().value() == 123456);
        REQUIRE(q2.Pop().value() == 99);
    }
}

TEST_CASE("Clear queue", "[queue]") {
    using namespace socket_server;

    MutexQueue<int32_t> q;

    q.Push(1000);
    q.Push(2000);
    q.Push(3000);
    q.Push(4000);

    REQUIRE(q.Size() == 4);
    REQUIRE_NOTHROW(q.Clear());
    REQUIRE(q.Size() == 0);
}

TEST_CASE("Init thread pool", "[threadpool]") {
    using namespace socket_server;

    ThreadPool pool(4);
}
