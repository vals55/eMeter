-- phpMyAdmin SQL Dump
-- version 5.2.0
-- https://www.phpmyadmin.net/
--
-- Хост: localhost
-- Время создания: Авг 25 2023 г., 20:26
-- Версия сервера: 5.6.43-84.3-log
-- Версия PHP: 8.0.27

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `bezru`
--

-- --------------------------------------------------------

--
-- Структура таблицы `emeter_stat`
--

CREATE TABLE `emeter_stat` (
  `id` int(11) NOT NULL,
  `voltage0` double DEFAULT NULL,
  `current0` double DEFAULT NULL,
  `power0` double DEFAULT NULL,
  `frequency` double DEFAULT NULL,
  `energy0` double DEFAULT NULL,
  `pf` double DEFAULT NULL,
  `imp01` int(11) DEFAULT NULL,
  `imp02` int(11) DEFAULT NULL,
  `imp1` int(11) DEFAULT NULL,
  `imp2` int(11) DEFAULT NULL,
  `voltage1` double DEFAULT NULL,
  `current1` double DEFAULT NULL,
  `power1` double DEFAULT NULL,
  `energy1` double DEFAULT NULL,
  `voltage2` double DEFAULT NULL,
  `current2` double DEFAULT NULL,
  `power2` double DEFAULT NULL,
  `energy2` double DEFAULT NULL,
  `rssi` int(11) DEFAULT NULL,
  `mac` varchar(20) DEFAULT NULL,
  `ip` varchar(20) DEFAULT NULL,
  `ver` int(11) DEFAULT NULL,
  `ver_esp` varchar(10) DEFAULT NULL,
  `chip_id` int(11) DEFAULT NULL,
  `freemem` int(11) DEFAULT NULL,
  `mqtt_period` int(11) DEFAULT NULL,
  `stat_period` int(11) DEFAULT NULL,
  `timestamp` timestamp NULL DEFAULT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Дамп данных таблицы `emeter_stat`
--

INSERT INTO `emeter_stat` (`id`, `voltage0`, `current0`, `power0`, `frequency`, `energy0`, `pf`, `imp01`, `imp02`, `imp1`, `imp2`, `voltage1`, `current1`, `power1`, `energy1`, `voltage2`, `current2`, `power2`, `energy2`, `rssi`, `mac`, `ip`, `ver`, `ver_esp`, `chip_id`, `freemem`, `mqtt_period`, `stat_period`, `timestamp`, `stamp`) VALUES
(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 220, 0, 0, 0, 220, 0, 0, 0, -65, 'C4:5B:BE:42:CD:B2', '192.168.254.109', 1, '0.40', 4378034, 31360, 10, 60, '2023-08-25 13:54:33', '2023-08-25 16:54:33'),
(2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 220, 0, 0, 0, 220, 0, 0, 0, -65, 'C4:5B:BE:42:CD:B2', '192.168.254.109', 1, '0.40', 4378034, 32320, 10, 60, '2023-08-25 13:55:33', '2023-08-25 16:55:33'),
(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 220, 0, 0, 0, 220, 0, 0, 0, -63, 'C4:5B:BE:42:CD:B2', '192.168.254.109', 1, '0.40', 4378034, 32136, 10, 60, '2023-08-25 14:16:04', '2023-08-25 17:16:04');

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `emeter_stat`
--
ALTER TABLE `emeter_stat`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `emeter_stat`
--
ALTER TABLE `emeter_stat`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
